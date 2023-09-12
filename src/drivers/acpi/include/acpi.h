/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#ifndef _ACPI_H_
#define _ACPI_H_

#include <stdint.h>

#define ACPI_REVISION 0x0000000000000000

#define ACPI_EMPTY 0
#define ACPI_INTEGER 1
#define ACPI_STRING 2
#define ACPI_BUFFER 3
#define ACPI_PACKAGE 4
#define ACPI_FIELD_UNIT 5
#define ACPI_DEVICE 6
#define ACPI_EVENT 7
#define ACPI_METHOD 8
#define ACPI_MUTEX 9
#define ACPI_REGION 10
#define ACPI_POWER 11
#define ACPI_PROCESSOR 12
#define ACPI_ALIAS 17
#define ACPI_SCOPE 18
#define ACPI_REFERENCE 19

#define ACPI_FIELD 0
#define ACPI_INDEX_FIELD 1

struct AcpiPackageElement;
struct AcpiObject;

typedef struct AcpiValue {
    int Type;
    struct AcpiObject *Objects;
    union {
        struct AcpiObject *Alias;
        struct AcpiObject *Reference;
        uint64_t Integer;
        char *String;
        struct {
            uint64_t Size;
            uint8_t *Data;
        } Buffer;
        struct {
            uint8_t Size;
            struct AcpiPackageElement *Data;
        } Package;
        struct {
            const uint8_t *Start;
            uint32_t Size;
            uint8_t Flags;
        } Method;
        struct {
            uint8_t Flags;
        } Mutex;
        struct {
            uint8_t RegionSpace;
            uint64_t RegionOffset;
            uint64_t RegionLen;
        } Region;
        struct {
            int FieldType;
            union {
                struct AcpiObject *Region;
                struct AcpiObject *Index;
            };
            struct AcpiObject *Data;
            uint8_t AccessType;
            uint8_t AccessAttrib;
            uint8_t AccessLength;
            uint32_t Offset;
            uint32_t Length;
        } Field;
        struct {
            uint8_t ProcId;
            uint32_t PblkAddr;
            uint8_t PblkLen;
        } Processor;
        struct {
            uint8_t SystemLevel;
            uint16_t ResourceOrder;
        } Power;
    };
} AcpiValue;

typedef struct AcpiPackageElement {
    int Type;
    union {
        char *String;
        AcpiValue Value;
    };
} AcpiPackageElement;

typedef struct AcpiObject {
    char Name[4];
    struct AcpiValue Value;
    struct AcpiObject *Next;
    struct AcpiObject *Parent;
} AcpiObject;

AcpiObject *AcpiSearchObject(const char *Name);
AcpiValue *AcpiExecuteMethodFromPath(const char *Name, int ArgCount, AcpiValue *Arguments);
AcpiValue *AcpiExecuteMethodFromObject(AcpiObject *Object, int ArgCount, AcpiValue *Arguments);

void AcpiFreeValueData(AcpiValue *Value);
void AcpiFreeValue(AcpiValue *Value);

#endif /* _ACPI_H_ */
