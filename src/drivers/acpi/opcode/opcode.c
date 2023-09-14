/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <acpip.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRY_EXECUTE_OPCODE(Function, ...)                \
    Status = Function(State, FullOpcode, ##__VA_ARGS__); \
    if (!Status) {                                       \
        return 0;                                        \
    } else if (Status > 0) {                             \
        break;                                           \
    }

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function executes the whichever AML opcode the current scope points to, updating the
 *     internal state accordingly.
 *
 * PARAMETERS:
 *     State - AML state containing the current scope.
 *     Result - Output; Where to store the resulting value; Only used for expression calls.
 *
 * RETURN VALUE:
 *     1 on success, 0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int AcpipExecuteOpcode(AcpipState *State, AcpiValue *Result) {
    uint8_t Opcode;
    if (!AcpipReadByte(State, &Opcode)) {
        return 0;
    }

    uint8_t ExtOpcode = 0;
    if (Opcode == 0x5B && !AcpipReadByte(State, &ExtOpcode)) {
        return 0;
    }

    uint16_t FullOpcode = Opcode | ((uint16_t)ExtOpcode << 8);
    AcpiValue Value;
    memset(&Value, 0, sizeof(AcpiValue));

    do {
        int Status;

        TRY_EXECUTE_OPCODE(AcpipExecuteConcatOpcode, &Value);
        TRY_EXECUTE_OPCODE(AcpipExecuteConvOpcode, &Value);
        TRY_EXECUTE_OPCODE(AcpipExecuteDataObjOpcode, &Value);
        TRY_EXECUTE_OPCODE(AcpipExecuteFieldOpcode);
        TRY_EXECUTE_OPCODE(AcpipExecuteMathOpcode, &Value);
        TRY_EXECUTE_OPCODE(AcpipExecuteNamedObjOpcode);
        TRY_EXECUTE_OPCODE(AcpipExecuteNsModOpcode);
        TRY_EXECUTE_OPCODE(AcpipExecuteStmtOpcode);

        switch (FullOpcode) {
            /* LocalObj (Local0-6) */
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66: {
                memcpy(&Value, &State->Locals[Opcode - 0x60], sizeof(AcpiValue));
                break;
            }

            /* ArgObj (Arg0-6) */
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E: {
                memcpy(&Value, &State->Arguments[Opcode - 0x68], sizeof(AcpiValue));
                break;
            }

            /* DefSizeOf := SizeOfOp SuperName */
            case 0x87: {
                AcpipTarget *SuperName = AcpipExecuteSuperName(State);
                if (!SuperName) {
                    return 0;
                }

                AcpiValue *Target = AcpipReadTarget(State, SuperName);
                if (!Target) {
                    free(SuperName);
                    return 0;
                }

                free(SuperName);
                Value.Type = ACPI_INTEGER;

                if (Target->Type == ACPI_STRING) {
                    Value.Integer = strlen(Target->String);
                } else if (Target->Type == ACPI_BUFFER) {
                    Value.Integer = Target->Buffer.Size;
                } else if (Target->Type == ACPI_PACKAGE) {
                    Value.Integer = Target->Package.Size;
                } else {
                    return 0;
                }

                break;
            }

            default: {
                State->Scope->Code--;
                State->Scope->RemainingLength++;

                /* MethodInvocation := NameString TermArgList
                   NameString should start with either a prefix (\, ^, 0x2E or 0x2F), or a lead
                   name char, so we can quickly check if this is it. */
                if (Opcode != '\\' && Opcode != '^' && Opcode != 0x2E && Opcode != 0x2F &&
                    !isupper(Opcode) && Opcode != '_') {
                    printf(
                        "unimplemented opcode: %#hx; %d bytes left to parse out of %d.\n",
                        Opcode | ((uint32_t)ExtOpcode << 8),
                        State->Scope->RemainingLength,
                        State->Scope->Length);
                    while (1)
                        ;
                }

                AcpipName *Name = AcpipReadName(State);
                if (!Name) {
                    return 0;
                }

                AcpiObject *Object = AcpipResolveObject(Name);
                if (!Object) {
                    free(Name);
                    return 0;
                }

                /* MethodInvocation is valid on non-method items, where we just return their
                   value. */
                if (Object->Value.Type != ACPI_METHOD) {
                    if (Object->Value.Type <= ACPI_FIELD_UNIT ||
                        Object->Value.Type == ACPI_REFERENCE ||
                        Object->Value.Type == ACPI_BUFFER_FIELD) {
                        memcpy(&Value, &Object->Value, sizeof(AcpiValue));
                    } else {
                        Value.Type = ACPI_REFERENCE;
                        Value.Reference = Object;
                    }

                    break;
                }

                AcpiValue Arguments[7];
                memset(Arguments, 0, sizeof(Arguments));

                /* The amount of arguments can be determined by using the method itself, which is a
                   bit annoying, as we can't parse the MethodInvocation or anything after it
                   (inside its enclosing scope) without knowing the method itself first. */
                for (int i = 0; i < (Object->Value.Method.Flags & 0x07); i++) {
                    if (!AcpipExecuteOpcode(State, &Arguments[i])) {
                        return 0;
                    }
                }

                AcpiValue *ReturnValue = AcpiExecuteMethodFromObject(
                    Object, Object->Value.Method.Flags & 0x07, Arguments);
                if (!ReturnValue) {
                    return 0;
                }

                memcpy(&Value, ReturnValue, sizeof(AcpiValue));
                free(ReturnValue);

                break;
            }
        }
    } while (0);

    if (Result) {
        memcpy(Result, &Value, sizeof(AcpiValue));
    }

    return 1;
}