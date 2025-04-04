; ModuleID = 'lua'
source_filename = "lua"

%somaP_return = type { double, double }

@fmt = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

; Function Attrs: noinline optnone
define %somaP_return @somaP(double %x1, double %y1, double %x2, double %y2) #0 {
entry:
  %y24 = alloca double, align 8
  %x23 = alloca double, align 8
  %y12 = alloca double, align 8
  %x11 = alloca double, align 8
  store double %x1, ptr %x11, align 8
  store double %y1, ptr %y12, align 8
  store double %x2, ptr %x23, align 8
  store double %y2, ptr %y24, align 8
  %x15 = load double, ptr %x11, align 8
  %x26 = load double, ptr %x23, align 8
  %addtmp = fadd double %x15, %x26
  %y17 = load double, ptr %y12, align 8
  %y28 = load double, ptr %y24, align 8
  %addtmp9 = fadd double %y17, %y28
  %0 = insertvalue %somaP_return undef, double %addtmp, 0
  %1 = insertvalue %somaP_return %0, double %addtmp9, 1
  ret %somaP_return %1
}

; Function Attrs: noinline optnone
define double @norma(double %x, double %y) #0 {
entry:
  %y2 = alloca double, align 8
  %x1 = alloca double, align 8
  store double %x, ptr %x1, align 8
  store double %y, ptr %y2, align 8
  %x3 = load double, ptr %x1, align 8
  %x4 = load double, ptr %x1, align 8
  %multmp = fmul double %x3, %x4
  %y5 = load double, ptr %y2, align 8
  %y6 = load double, ptr %y2, align 8
  %multmp7 = fmul double %y5, %y6
  %addtmp = fadd double %multmp, %multmp7
  ret double %addtmp
}

; Function Attrs: noinline optnone
define double @retorno_multiplo() #0 {
entry:
  %somaP_result = call %somaP_return @somaP(double 2.000000e+00, double 3.000000e+00, double 4.000000e+00, double 5.000000e+00)
  %0 = extractvalue %somaP_return %somaP_result, 0
  %1 = extractvalue %somaP_return %somaP_result, 1
  %norma_result = call double @norma(double %0, double %1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt, double %norma_result)
  ret double 0.000000e+00
}

define i32 @main() {
entry:
  %retorno_multiplo_result = call double @retorno_multiplo()
  ret i32 0
}

attributes #0 = { noinline optnone }
