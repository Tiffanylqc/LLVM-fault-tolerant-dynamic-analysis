; ModuleID = '<stdin>'
source_filename = "/home/qingcanl/Documents/a5/se-fault-tolerant-template/test/c/store-global-byte.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@bytes = global [5 x i8] c"\03\05\07\09\0B", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i64 @storeByte(i32, i8 zeroext) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i8, align 1
  %5 = alloca i64, align 8
  %6 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i8 %1, i8* %4, align 1
  %7 = load i8, i8* %4, align 1
  %8 = load i32, i32* %3, align 4
  %9 = zext i32 %8 to i64
  %10 = getelementptr inbounds [5 x i8], [5 x i8]* @bytes, i64 0, i64 %9
  store i8 %7, i8* %10, align 1
  store i64 0, i64* %5, align 8
  store i32 0, i32* %6, align 4
  br label %11

; <label>:11:                                     ; preds = %23, %2
  %12 = load i32, i32* %6, align 4
  %13 = icmp ult i32 %12, 5
  br i1 %13, label %14, label %26

; <label>:14:                                     ; preds = %11
  %15 = load i64, i64* %5, align 8
  %16 = mul i64 3, %15
  %17 = load i32, i32* %6, align 4
  %18 = zext i32 %17 to i64
  %19 = getelementptr inbounds [5 x i8], [5 x i8]* @bytes, i64 0, i64 %18
  %20 = load i8, i8* %19, align 1
  %21 = zext i8 %20 to i64
  %22 = add i64 %16, %21
  store i64 %22, i64* %5, align 8
  br label %23

; <label>:23:                                     ; preds = %14
  %24 = load i32, i32* %6, align 4
  %25 = add i32 %24, 1
  store i32 %25, i32* %6, align 4
  br label %11

; <label>:26:                                     ; preds = %11
  store i8 3, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i64 0, i64 0), align 1
  store i8 5, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i64 0, i64 1), align 1
  store i8 7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i64 0, i64 2), align 1
  store i8 9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i64 0, i64 3), align 1
  store i8 11, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i64 0, i64 4), align 1
  %27 = load i64, i64* %5, align 8
  ret i64 %27
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.1-svn334776-1~exp1~20190309042703.125 (branches/release_60)"}
