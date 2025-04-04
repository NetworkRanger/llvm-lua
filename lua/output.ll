; ModuleID = 'lua'
source_filename = "lua"

@format = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @format, double 3.000000e+00)
  ret i32 %0
}
