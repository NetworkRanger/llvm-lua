; ModuleID = 'lua'
source_filename = "lua"

@0 = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1
@fmt = private unnamed_addr constant [3 x i8] c"%g\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt, ptr @0)
  ret i32 0
}
