	AREA text, CODE, READONLY
		EXPORT Kalmanfilter_asm
			
Kalmanfilter_asm
		
	; initialize q (S1), r(S2) and p(S3) to 0.1	
	VMOV.F32 S1, #10
	VMOV.F32 S2, #1
	VDIV.F32 S1, S2, S1 ; TO GET 0.1, TRY LOAD FROM MEMORY, ADD, SUBT, DIV...
	
	VMOV.F32 S2, S1
	VMOV.F32 S3, S1

	; initialize k (S4) and x (S5) to 0.
	VSUB.F32 S4, S1, S2
	VMOV.F32 S5, S4
	
	VMOV.F32 S0, #0.125 ; FOR TESING ONLY~
	
	; update
	VADD.F32 S3, S3, S1
	VADD.F32 S5, S3, S1 ;P+R
	VDIV.F32 S4, S3, S5
	VSUB.F32 S6, S0, S5 ;measurement will be written in S0
	VMUL.F32 S7, S4, S6
	VADD.F32 S5, S5, S7
	VNEG.F32 S8, S4
	VADD.F32 S8, S8, S4
	VMUL.F32 S3, S8, S3
	
	
	BX	LR;

	END