; ModuleID = '<stdin>'
source_filename = "/home/qingcanl/Documents/a5/se-fault-tolerant-template/test/c/store-local-byte-array.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@storeByte.bytes = private unnamed_addr constant [5 x i8] c"\03\05\07\09\0B", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i64 @storeByte(i32, i8 zeroext) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i8, align 1
  %5 = alloca [5 x i8], align 1
  %6 = alloca i64, align 8
  %7 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i8 %1, i8* %4, align 1
  %8 = bitcast [5 x i8]* %5 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %8, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @storeByte.bytes, i32 0, i32 0), i64 5, i32 1, i1 false)
  %9 = load i8, i8* %4, align 1
  %10 = load i32, i32* %3, align 4
  %11 = zext i32 %10 to i64
  %12 = getelementptr inbounds [5 x i8], [5 x i8]* %5, i64 0, i64 %11
  store i8 %9, i8* %12, align 1
  store i64 0, i64* %6, align 8
  store i32 0, i32* %7, align 4
  br label %13

; <label>:13:                                     ; preds = %25, %2
  %14 = load i32, i32* %7, align 4
  %15 = icmp ult i32 %14, 5
  br i1 %15, label %16, label %28

; <label>:16:                                     ; preds = %13
  %17 = load i64, i64* %6, align 8
  %18 = mul i64 3, %17
  %19 = load i32, i32* %7, align 4
  %20 = zext i32 %19 to i64
  %21 = getelementptr inbounds [5 x i8], [5 x i8]* %5, i64 0, i64 %20
  %22 = load i8, i8* %21, align 1
  %23 = zext i8 %22 to i64
  %24 = add i64 %18, %23
  store i64 %24, i64* %6, align 8
  br label %25

; <label>:25:                                     ; preds = %16
  %26 = load i32, i32* %7, align 4
  %27 = add i32 %26, 1
  store i32 %27, i32* %7, align 4
  br label %13

; <label>:28:                                     ; preds = %13
  %29 = load i64, i64* %6, align 8
  ret i64 %29
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.1-svn334776-1~exp1~20190309042703.125 (branches/release_60)"}
