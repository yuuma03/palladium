/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <acpip.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *Types[] = {
    "[Uninitialized Object]",
    "[Integer]",
    "[String]",
    "[Buffer]",
    "[Package]",
    "[Field]",
    "[Device]",
    "[Event]",
    "[Control Method]",
    "[Mutex]",
    "[Operation Region]",
    "[Power Resource]",
    "[Processor]",
    "[Thermal Zone]",
    "[Buffer Field]",
    "[Reserved]",
    "[Debug Object]",
    "[Scope]",
};

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function tries casting the result of a previous ExecuteOpcode into an integer.
 *
 * PARAMETERS:
 *     Value - Result of ExecuteOpcode.
 *     Result - Output as an integer.
 *
 * RETURN VALUE:
 *     1 on success, 0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int AcpipCastToInteger(AcpiValue *Value, uint64_t *Result) {
    if (Value->Type == ACPI_INTEGER) {
        *Result = Value->Integer;
        return 1;
    }

    *Result = 0;
    switch (Value->Type) {
        /* The contents of the buffer should be copied 1-to-1 into the integer, making sure to
           validate the size of the buffer. */
        case ACPI_BUFFER:
            for (uint64_t i = 0; i < (Value->Buffer.Size > 8 ? 8 : Value->Buffer.Size); i++) {
                *Result |= Value->Buffer.Data[i] << (i * 8);
            }

            break;

        /* The string is copied assuming every position is a hex-character; We have strtoull, which
           does the conversion for us. */
        case ACPI_STRING:
            *Result = strtoull(Value->String, NULL, 16);
            break;

        default:
            return 0;
    }

    return 1;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function tries casting the result of a previous ExecuteOpcode into a string.
 *
 * PARAMETERS:
 *     Value - I/O; Result of ExecuteOpcode; Will become the converted string after the cast.
 *     ImplicitCast - Set this to 0 if you only want explicit casts, or to 1 otherwise.
 *     Decimal - Set this to 0 if you want decimal digits instead of hexadecimal, or to 1
 *               otherwise.
 *
 * RETURN VALUE:
 *     1 on success, 0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int AcpipCastToString(AcpiValue *Value, int ImplicitCast, int Decimal) {
    if (Value->Type == ACPI_STRING) {
        return 1;
    }

    char *String;
    switch (Value->Type) {
        /* Integers have two conversion paths:
               For hexadecimal, they get converted using snprintf into 16 hex digits.
               For decimal, we need snprintf to get the output size, then we convert it. */
        case ACPI_INTEGER: {
            if (Decimal) {
                int Size = snprintf(NULL, 0, "%lld", Value->Integer);
                String = malloc(Size + 1);
                if (!String) {
                    return 0;
                }

                if (snprintf(String, Size + 1, "%lld", Value->Integer) != Size) {
                    free(String);
                    return 0;
                }
            } else {
                String = malloc(17);
                if (!String) {
                    return 0;
                }

                if (snprintf(String, 17, "%016llX", Value->Integer) != 16) {
                    free(String);
                    return 0;
                }
            }

            break;
        }

        /* Buffers should be converted into a list of either space or comma separated pairs of
           digits; We use space for implicit conversion, and comma for explicit. */
        case ACPI_BUFFER: {
            /* Hexadecimal is the easier case, as we know we should always have 4 digits per
               buffer item; For decimal, we're not sure of the size until we run snprintf, so
               we need two loops using snprintf. */
            if (Decimal) {
                int Size = 0;

                for (uint64_t i = 0; i < Value->Buffer.Size; i++) {
                    const char *PrintfFormat = "%lld";

                    if (i < Value->Buffer.Size - 1) {
                        PrintfFormat = ImplicitCast ? "%lld " : "%lld,";
                    }

                    Size += snprintf(NULL, 0, PrintfFormat, Value->Buffer.Data[i]);
                }

                String = malloc(Size + 1);
                if (!String) {
                    return 0;
                }

                int Offset = 0;
                for (uint64_t i = 0; i < Value->Buffer.Size; i++) {
                    const char *PrintfFormat = "%lld";

                    if (i < Value->Buffer.Size - 1) {
                        PrintfFormat = ImplicitCast ? "%lld " : "%lld,";
                    }

                    Offset += snprintf(
                        String + Offset, Size - Offset + 1, PrintfFormat, Value->Buffer.Data[i]);
                }
            } else {
                String = malloc(Value->Buffer.Size * 5);
                if (!String) {
                    return 0;
                }

                for (uint64_t i = 0; i < Value->Buffer.Size; i++) {
                    int PrintfSize = 5;
                    const char *PrintfFormat = "0x%02hhX";

                    if (i < Value->Buffer.Size - 1) {
                        PrintfSize = 6;
                        PrintfFormat = ImplicitCast ? "0x%02hhX " : "0x%02hhX,";
                    }

                    if (snprintf(
                            Value->String + i * 5,
                            PrintfSize,
                            PrintfFormat,
                            Value->Buffer.Data[i]) != PrintfSize - 1) {
                        free(String);
                        return 0;
                    }
                }
            }

            break;
        }

        /* For everything else, we'll just be converting their type into a string, and returning
           that. */
        default: {
            String = strdup(Types[Value->Type]);
            if (!String) {
                return 0;
            }

            break;
        }
    }

    Value->Type = ACPI_STRING;
    Value->String = String;

    return 1;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function tries casting the result of a previous ExecuteOpcode into a buffer.
 *
 * PARAMETERS:
 *     Value - I/O; Result of ExecuteOpcode; Will become the converted buffer after the cast.
 *
 * RETURN VALUE:
 *     1 on success, 0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int AcpipCastToBuffer(AcpiValue *Value) {
    if (Value->Type == ACPI_BUFFER) {
        return 1;
    }

    uint64_t BufferSize = 0;
    uint8_t *Buffer = NULL;

    switch (Value->Type) {
        /* Integers get their underlying in-memory representation copied as an 8-byte buffer. */
        case ACPI_INTEGER: {
            BufferSize = 16;
            Buffer = malloc(16);

            if (!Buffer) {
                return 0;
            }

            *((uint64_t *)Buffer) = Value->Integer;
            break;
        }

        /* Strings mostly pass through, except for 0-length, they just become 0-sized buffers
           (instead of length==1). */
        case ACPI_STRING: {
            size_t StringSize = strlen(Value->String);

            if (StringSize) {
                BufferSize = StringSize + 1;
                Buffer = (uint8_t *)strdup(Value->String);
                if (!Buffer) {
                    return 0;
                }
            }

            break;
        }

        default:
            return 0;
    }

    Value->Type = ACPI_BUFFER;
    Value->Buffer.Size = BufferSize;
    Value->Buffer.Data = Buffer;

    return 1;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function tries executing the next termarg in the scope.
 *
 * PARAMETERS:
 *     State - AML state containing the current scope.
 *     Value - Output as an integer.
 *
 * RETURN VALUE:
 *     1 on success, 0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int AcpipExecuteInteger(AcpipState *State, uint64_t *Result) {
    AcpiValue Value;
    if (!AcpipExecuteOpcode(State, &Value)) {
        return 0;
    }

    return AcpipCastToInteger(&Value, Result);
}