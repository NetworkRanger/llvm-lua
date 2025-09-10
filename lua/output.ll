; ModuleID = 'lua'
source_filename = "lua"

@fmt = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %i = alloca double, align 8
  store double 1.000000e+00, ptr %i, align 8
  br label %loop1

loop1:                                            ; preds = %loopinc, %entry
  %i2 = load double, ptr %i, align 8
  %loopcond = fcmp ule double %i2, 3.000000e+00
  br i1 %loopcond, label %loopbody, label %afterloop

loopbody:                                         ; preds = %loop1
  %i3 = load double, ptr %i, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt, double %i3)
  br label %loopinc

loopinc:                                          ; preds = %loopbody
  %nextvar = fadd double %i2, 1.000000e+00
  store double %nextvar, ptr %i, align 8
  br label %loop1

afterloop:                                        ; preds = %loop1
  ret i32 0
}
