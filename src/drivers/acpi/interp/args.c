/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <acpip.h>

/* Abbreviations for the ACPI_ARG values; Used to make the table lines a bit smaller. */
#define _ ACPI_ARG_NONE  // Representing an empty space
#define B ACPI_ARG_BYTE
#define W ACPI_ARG_WORD
#define D ACPI_ARG_DWORD
#define Q ACPI_ARG_QWORD
#define S ACPI_ARG_STRING
#define N ACPI_ARG_NAME
#define T ACPI_ARG_TERM_ARG
#define O ACPI_ARG_OBJ_REF  // always strong, unless the parent op is CondRefOf
#define I ACPI_ARG_INTEGER
#define U ACPI_ARG_BUFFER  // bUffer
#define P ACPI_ARG_PACKAGE

/* We separate the opcodes into two groups:
       Group 0, single byte + arguments.
       Group 1, two bytes (0x5B and opcode) + arguments. */

AcpipArgument AcpipGroup0Arguments[256] = {
    {1, 0, 0, {_, _, _, _, _, _}},  // ZeroOp
    {1, 0, 0, {_, _, _, _, _, _}},  // OneOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 2, 0, {N, N, _, _, _, _}},  // AliasOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 2, 0, {N, O, _, _, _, _}},  // NameOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 1, 0, {B, _, _, _, _, _}},  // BytePrefix
    {1, 1, 0, {W, _, _, _, _, _}},  // WordPrefix
    {1, 1, 0, {D, _, _, _, _, _}},  // DWordPrefix
    {1, 1, 0, {S, _, _, _, _, _}},  // StringPrefix
    {1, 1, 0, {Q, _, _, _, _, _}},  // QWordPrefix
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 1, 1, {N, _, _, _, _, _}},  // ScopeOp
    {1, 1, 1, {I, _, _, _, _, _}},  // BufferOp
    {1, 1, 1, {B, _, _, _, _, _}},  // PackageOp
    {1, 1, 1, {I, _, _, _, _, _}},  // VarPackageOp
    {1, 2, 1, {N, B, _, _, _, _}},  // MethodOp
    {1, 3, 0, {N, B, B, _, _, _}},  // ExternMalOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // DualNamePrefix
    {1, 0, 0, {_, _, _, _, _, _}},  // MultiNamePrefix
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  // DigitChar
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChMar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {0, 0, 0, {_, _, _, _, _, _}},  // ExtOpPrefix
    {1, 0, 0, {_, _, _, _, _, _}},  // RootChar
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // ParentPrefixChar
    {1, 0, 0, {_, _, _, _, _, _}},  // NameChar
    {1, 0, 0, {_, _, _, _, _, _}},  // Local0Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local1Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local2Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local3Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local4Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local5Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local6Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Local7Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg0Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg1Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg2Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg3Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg4Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg5Op
    {1, 0, 0, {_, _, _, _, _, _}},  // Arg6Op
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 2, 0, {T, O, _, _, _, _}},  // StoreOp
    {1, 1, 0, {O, _, _, _, _, _}},  // RefOfOp
    {1, 3, 0, {I, I, O, _, _, _}},  // AddOp
    {1, 3, 0, {T, T, O, _, _, _}},  // ConcatOp
    {1, 3, 0, {I, I, O, _, _, _}},  // SubtractOp
    {1, 1, 0, {O, _, _, _, _, _}},  // IncrementOp
    {1, 1, 0, {O, _, _, _, _, _}},  // DecrementOp
    {1, 3, 0, {I, I, O, _, _, _}},  // MultiplyOp
    {1, 4, 0, {I, I, O, O, _, _}},  // DivideOp
    {1, 3, 0, {I, I, O, _, _, _}},  // ShiftLeftOp
    {1, 3, 0, {I, I, O, _, _, _}},  // ShiftRightOp
    {1, 3, 0, {I, I, O, _, _, _}},  // AndOp
    {1, 3, 0, {I, I, O, _, _, _}},  // NandOp
    {1, 3, 0, {I, I, O, _, _, _}},  // OrOp
    {1, 3, 0, {I, I, O, _, _, _}},  // NorOp
    {1, 3, 0, {I, I, O, _, _, _}},  // XorOp
    {1, 2, 0, {I, O, _, _, _, _}},  // NotOp
    {1, 2, 0, {I, O, _, _, _, _}},  // FindSetLeftBitOp
    {1, 2, 0, {I, O, _, _, _, _}},  // FindSetRightBitOp
    {1, 1, 0, {O, _, _, _, _, _}},  // DerefOfOp
    {1, 3, 0, {T, T, O, _, _, _}},  // ConcatResOp
    {1, 3, 0, {I, I, O, _, _, _}},  // ModOp
    {1, 2, 0, {O, I, _, _, _, _}},  // NotifyOp
    {1, 1, 0, {O, _, _, _, _, _}},  // SizeOfOp
    {1, 3, 0, {T, I, O, _, _, _}},  // IndexOp
    {1, 6, 0, {P, B, I, B, I, I}},  // MatchOp
    {1, 3, 0, {U, I, N, _, _, _}},  // CreateDWordFieldOp
    {1, 3, 0, {U, I, N, _, _, _}},  // CreateWordFieldOp
    {1, 3, 0, {U, I, N, _, _, _}},  // CreateByteFieldOp
    {1, 3, 0, {U, I, N, _, _, _}},  // CreateBitFieldOp
    {1, 1, 0, {O, _, _, _, _, _}},  // ObjectTypeOp
    {1, 3, 0, {U, I, N, _, _, _}},  // CreateQWordFieldOp
    {1, 2, 0, {T, T, _, _, _, _}},  // LandOp
    {1, 2, 0, {T, T, _, _, _, _}},  // LorOp
    {1, 1, 0, {T, _, _, _, _, _}},  // LnotOp
    {1, 2, 0, {T, T, _, _, _, _}},  // LEqualOp
    {1, 2, 0, {T, T, _, _, _, _}},  // LGreaterOp
    {1, 2, 0, {T, T, _, _, _, _}},  // LLessOp
    {1, 2, 0, {T, O, _, _, _, _}},  // ToBufferOp
    {1, 2, 0, {T, O, _, _, _, _}},  // ToDecimalStringOp
    {1, 2, 0, {T, O, _, _, _, _}},  // ToHexStringOp
    {1, 2, 0, {T, O, _, _, _, _}},  // ToIntegerOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 3, 0, {T, I, O, _, _, _}},  // ToStringOp
    {1, 2, 0, {T, O, _, _, _, _}},  // CopyObjectOp
    {1, 4, 0, {T, T, T, O, _, _}},  // MidOp
    {1, 0, 0, {_, _, _, _, _, _}},  // ContinueOp
    {1, 1, 1, {I, _, _, _, _, _}},  // IfOp
    {1, 0, 1, {_, _, _, _, _, _}},  // ElseOp
    {1, 1, 1, {I, _, _, _, _, _}},  // WhileOp
    {1, 0, 0, {_, _, _, _, _, _}},  // NoopOp
    {1, 1, 0, {T, _, _, _, _, _}},  // ReturnOp
    {1, 0, 0, {_, _, _, _, _, _}},  // BreakOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // BreakPointOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // OnesOp
};

AcpipArgument AcpipGroup1Arguments[256] = {
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 2, 0, {N, B, _, _, _, _}},  // MutexOp
    {1, 1, 0, {N, _, _, _, _, _}},  // EventOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 2, 0, {O, O, _, _, _, _}},  // CondRefOfOp
    {1, 4, 0, {U, I, I, N, _, _}},  // CreateFieldOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 6, 0, {T, T, T, T, T, T}},  // LoadTableOp
    {1, 2, 0, {N, O, _, _, _, _}},  // LoadOp
    {1, 1, 0, {I, _, _, _, _, _}},  // StallOp
    {1, 1, 0, {I, _, _, _, _, _}},  // SleepOp
    {1, 2, 0, {O, W, _, _, _, _}},  // AcquireOp
    {1, 1, 0, {O, _, _, _, _, _}},  // SignalOp
    {1, 2, 0, {O, I, _, _, _, _}},  // WaitOp
    {1, 1, 0, {O, _, _, _, _, _}},  // ResetOp
    {1, 1, 0, {O, _, _, _, _, _}},  // ReleaseOp
    {1, 2, 0, {I, O, _, _, _, _}},  // FromBCDOp
    {1, 2, 0, {I, O, _, _, _, _}},  // ToBCDOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 0, 0, {_, _, _, _, _, _}},  // RevisionOp
    {1, 0, 0, {_, _, _, _, _, _}},  // DebugOp
    {1, 3, 0, {B, D, I, _, _, _}},  // FatalOp
    {1, 0, 0, {_, _, _, _, _, _}},  // TimerOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {1, 4, 0, {N, B, I, I, _, _}},  // OpRegionOp
    {1, 2, 1, {N, B, _, _, _, _}},  // FieldOp
    {1, 1, 1, {N, _, _, _, _, _}},  // DeviceOp
    {1, 4, 1, {N, B, D, B, _, _}},  // ProcessorOp
    {1, 3, 1, {N, B, W, _, _, _}},  // PowerResOp
    {1, 1, 1, {N, _, _, _, _, _}},  // ThermalZoneOp
    {1, 3, 1, {N, N, B, _, _, _}},  // IndexFieldOp
    {1, 4, 1, {N, N, I, B, _, _}},  // BankFieldOp
    {1, 4, 0, {N, T, T, T, _, _}},  // DataRegionOp
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
    {0, 0, 0, {_, _, _, _, _, _}},  //
};
