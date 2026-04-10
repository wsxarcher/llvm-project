// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-apple-macosx10.15.0 -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-windows-msvc -emit-llvm %s -o - | FileCheck %s

// Test that getLargeGlobalPreferredAlign does not bump alignment for extern
// global variable declarations. Extern variables are defined in external
// libraries, and the compiler cannot assume they are aligned beyond their
// natural type alignment.

// A 128-byte struct of doubles (natural alignment 8)
struct S128Doubles {
    double m11,m12,m13,m14;
    double m21,m22,m23,m24;
    double m31,m32,m33,m34;
    double m41,m42,m43,m44;
};

// A 128-byte struct of chars (natural alignment 1)
struct S128 {
    char Buffer[128];
};

// A 64-byte struct of chars (natural alignment 1)
struct S64 {
    char Buffer[64];
};

// A 16-byte struct of chars (natural alignment 1)
struct S16 {
    char Buffer[16];
};

// Extern declarations: alignment must NOT be bumped beyond the type's natural
// preferred alignment, because the compiler does not control placement of
// extern symbols.
extern struct S128Doubles extern_s128doubles;
extern struct S128 extern_s128;
extern struct S64 extern_s64;
extern struct S16 extern_s16;

// Definitions: alignment CAN be bumped because the compiler controls placement.
struct S128Doubles defined_s128doubles = {0};
struct S128 defined_s128 = {0};
struct S64 defined_s64 = {0};
struct S16 defined_s16 = {0};

// Extern globals should use the type's natural preferred alignment:
// CHECK-DAG: @extern_s128doubles = external {{(dso_local )?}}global %struct.S128Doubles, align 8
// CHECK-DAG: @extern_s128 = external {{(dso_local )?}}global %struct.S128, align 1
// CHECK-DAG: @extern_s64 = external {{(dso_local )?}}global %struct.S64, align 1
// CHECK-DAG: @extern_s16 = external {{(dso_local )?}}global %struct.S16, align 1

// Defined globals can use the bumped alignment:
// CHECK-DAG: @defined_s128doubles = {{(dso_local )?}}global %struct.S128Doubles zeroinitializer, align 16
// CHECK-DAG: @defined_s128 = {{(dso_local )?}}global %struct.S128 zeroinitializer, align 16
// CHECK-DAG: @defined_s64 = {{(dso_local )?}}global %struct.S64 zeroinitializer, align 8
// CHECK-DAG: @defined_s16 = {{(dso_local )?}}global %struct.S16 zeroinitializer, align 8

void use(void *);
void test(void) {
    use(&extern_s128doubles);
    use(&extern_s128);
    use(&extern_s64);
    use(&extern_s16);
    use(&defined_s128doubles);
    use(&defined_s128);
    use(&defined_s64);
    use(&defined_s16);
}
