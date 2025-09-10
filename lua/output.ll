; ModuleID = 'lua'
source_filename = "lua"

@fmt = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt, double 3.000000e+00)
  ret i32 0
}
