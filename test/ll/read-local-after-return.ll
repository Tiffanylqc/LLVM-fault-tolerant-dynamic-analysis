; ModuleID = '<stdin>'
source_filename = "/home/qingcanl/Documents/a5/se-fault-tolerant-template/test/c/read-local-after-return.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@readByteHelper.bytes = private unnamed_addr constant [5 x i8] c"\09\0B\0D\11\13", align 1
@readByte.bytes = private unnamed_addr constant [5 x i8] c"\09\0B\0D\11\13", align 1

; Function Attrs: noinline nounwind optnone uwtable
define i8* @readByteHelper(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [5 x i8], align 1
  store i32 %0, i32* %2, align 4
  %4 = bitcast [5 x i8]* %3 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %4, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @readByteHelper.bytes, i32 0, i32 0), i64 5, i32 1, i1 false)
  %5 = load i32, i32* %2, align 4
  %6 = zext i32 %5 to i64
  %7 = getelementptr inbounds [5 x i8], [5 x i8]* %3, i64 0, i64 %6
  ret i8* %7
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #1

; Function Attrs: noinline nounwind optnone uwtable
define zeroext i8 @readByte(i32, i32) #0 {
  %3 = alloca i8, align 1
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca [5 x i8], align 1
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  %7 = bitcast [5 x i8]* %6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @readByte.bytes, i32 0, i32 0), i64 5, i32 1, i1 false)
  %8 = load i32, i32* %5, align 4
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %10, label %15

; <label>:10:                                     ; preds = %2
  %11 = load i32, i32* %4, align 4
  %12 = zext i32 %11 to i64
  %13 = getelementptr inbounds [5 x i8], [5 x i8]* %6, i64 0, i64 %12
  %14 = load i8, i8* %13, align 1
  store i8 %14, i8* %3, align 1
  br label %19

; <label>:15:                                     ; preds = %2
  %16 = load i32, i32* %4, align 4
  %17 = call i8* @readByteHelper(i32 %16)
  %18 = load i8, i8* %17, align 1
  store i8 %18, i8* %3, align 1
  br label %19

; <label>:19:                                     ; preds = %15, %10
  %20 = load i8, i8* %3, align 1
  ret i8 %20
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.1-svn334776-1~exp1~20190309042703.125 (branches/release_60)"}
