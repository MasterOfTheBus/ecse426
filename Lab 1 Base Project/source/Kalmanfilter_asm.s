	AREA text, CODE, READONLY
	EXPORT Kalmanfilter_asm		
Kalmanfilter_asm 
		; CALL CONVENTION: 
		; R0 - OUTPUT ARRAY POINTER
		; R1 - INPUT ARRAY POINTER
		; R2 - ARRAY LENGTH
		; R3 - KALMAN FILTER STATE VARIABLE ARRAY POINTER
	
		; Store Kalman Filter state variables in their respective registers
		VLDM R3, {S1-S5}
		
		; Initialize counter and prepare for loop
		MOV R4, #1			; initialize counter to 1
		VMOV.F32 S9, #1		; Store 1 in register for computation uses
				
		; Loop through the input and write to memory
UPDATE	VLDR S0, [R1]			;operations are out of order - attempting to minimize (data dependency) stall during pipelining.
		VADD.F32 S4, S4, S1		;P = P+Q					1
		VADD.F32 S6, S4, S2 	;TEMP = P+R					2
		VDIV.F32 S5, S4, S6		;K = P/P+R					3	
		VSUB.F32 S6, S0, S3 	;TEMP = MEASUREMENT-X		4
		VNEG.F32 S8, S5			;TEMP = -K					7
		VMUL.F32 S7, S5, S6		;TEMP = K*(MEASUREMENT-X)	5
		VADD.F32 S8, S8, S9		;TEMP = 1-K					8
		VADD.F32 S3, S3, S7		;X = X+K*(MEASUREMENT-X)	6
		VMUL.F32 S4, S8, S4		;P = (1-K)*P				9
		VSTR S3, [R0]			; WRITE THE INPUT IN MEMORY
		ADD R0, R0, #4			; INCREMENT OUTPUT POINTER
		ADD R1, R1, #4			; INCREMENT INPUT POINTER
		ADD R4, #1				; INCREMENT COUNTER
		CMP R4, R2				; COMPARE ARRAY LENGTH VS COUNTER
		BLE UPDATE				; ARRAY LENGTH IS HIGHER THAN THE COUNTER, LOOP AGAIN

	BX	LR;