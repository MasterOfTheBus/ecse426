	AREA text, CODE, READONLY
	EXPORT Kalmanfilter_asm		
Kalmanfilter_asm 
		; CALL CONVENTION: 
		; R0 - OUTPUT ARRAY POINTER
		; R1 - INPUT ARRAY POINTER
		; R2 - ARRAY LENGTH
		; R3 - KALMAN FILTER STATE VARIABLE ARRAY POINTER
		
		; ***When calling the subroutine from C, remove steps 1 and 2!
		; Step 1* - Set the array length and the address where the output will be stores
		;MOV R2, #1257 ; array length
		;MOV R0, #0x20000000 ; store the output here
		;MOV R5, R0			; Use R5 as a pointer to load the results FOR DEBUGGING ONLY!
		
		; Step 2* - Find the Kalman Filter state variables and input data in memory
		;LDR R3, =qrxpk
		;LDR R1, =indata
		
		; Step 3 - Store Kalman Filter state variables in their respective registers
		VLDM R3, {S1-S5}
		
		; Step 4 - Initialize counter and prepare for loop
		MOV R4, #1			; initialize counter to 1
		
		VMOV.F32 S9, #1		; Store 1 in register for computation uses
				
		; Step 5 - Loop through the input and write to memory
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
		; Step 6* -  Write the fist 21 outputs to register for visualizing results
		;VLDM R5, {S10-S31}
		; Step 7* - Return result in R0
		;MOV R0, #1
	BX	LR;
	
	AREA input, DATA, READONLY
qrxpk	
	DCFS 0.005, 5.0, 0.0, 0.0, 0.0
indata
	DCFS -0.039358, -0.144414, -0.053388, -0.066994, 0.005027, 0.066903, 0.144760, -0.036963, 0.109277, 0.198527, 0.195965, -0.049666, 0.192914, 0.112757, 0.277684, 0.242368, 0.124434, 0.196125, 0.230115, -0.057259, 0.137271, 0.168915, 0.103754, 0.004741, 0.321706, 0.186025, 0.374296, 0.170907, 0.371668, 0.356297, 0.351510, 0.201259, 0.296735, 0.277049, 0.237498, 0.298395, 0.190157, 0.345876, 0.351175, 0.297052, 0.453152, 0.293749, 0.382942, 0.469362, 0.447262, 0.195228, 0.366706, 0.412276, 0.406447, 0.335329, 0.475114, 0.471414, 0.624839, 0.516755, 0.584684, 0.509648, 0.348640, 0.392304, 0.472884, 0.512988, 0.622278, 0.489422, 0.518156, 0.614730, 0.540990, 0.482344, 0.557713, 0.552958, 0.697909, 0.658731, 0.698811, 0.605859, 0.871056, 0.677780, 0.582305, 0.549278, 0.698786, 0.655471, 0.568090, 0.663372, 0.554555, 0.792529, 0.925517, 0.811955, 0.712325, 0.887245, 0.786946, 0.542429, 0.756276, 0.784942, 0.839549, 0.773076, 0.912972, 0.668780, 1.053479, 0.948636, 0.794514, 0.853687, 0.743230, 0.742950, 0.849087, 0.945398
	DCFS 0.944692, 1.070449, 0.829821, 0.845122, 0.598925, 0.846346, 0.896569, 0.890400, 0.826443, 0.766958, 0.858999, 1.124467, 0.815729, 0.913030, 0.801041, 1.037204, 1.003250, 0.944403, 0.927285, 1.083503, 0.996876, 0.942136, 0.959450, 1.102883, 1.001574, 0.981987, 1.014873, 0.950976, 0.864836, 1.002946, 0.806485, 0.901734, 1.079279, 0.906266, 1.004876, 0.958381, 1.009599, 1.026061, 0.935357, 0.774643, 1.090767, 0.934313, 1.019939, 1.089669, 0.885628, 1.120541, 1.069997, 1.008976, 0.961616, 1.118672, 1.043288, 1.052401, 1.268814, 1.055247, 1.117594, 0.994931, 1.180184, 0.990306, 0.900074, 0.997567, 0.827185, 1.124008, 0.879191, 1.025030, 1.087459, 1.043358, 1.031398, 0.906456, 0.986160, 1.083367, 0.912532, 0.863784, 0.915853, 1.069430, 0.954698, 1.002512, 0.803713, 0.898808, 0.903900, 0.844934, 1.086506, 1.070584, 0.807261, 0.986262, 0.861916, 0.938997, 0.957144, 1.120441, 0.983344, 0.877375, 1.022784, 1.023777, 0.946446, 1.153183, 0.908535, 0.875466, 0.972057, 0.783183, 0.868145, 0.851083, 1.037440, 0.967091, 0.952686
	DCFS 0.979344, 0.972647, 0.881236, 0.731547, 0.700129, 0.866987, 1.024464, 0.936040, 0.952443, 1.021712, 0.788387, 0.768021, 0.935975, 0.748402, 1.034343, 0.897558, 0.739028, 0.759215, 0.842668, 0.855606, 0.894914, 0.805749, 0.663882, 0.768195, 0.598710, 0.803327, 0.796211, 0.746899, 0.632812, 0.845779, 0.810265, 0.633225, 0.563847, 0.790499, 0.743654, 0.679056, 0.693591, 0.730297, 0.609923, 0.800826, 0.672430, 0.553382, 0.572888, 0.764828, 0.541468, 0.492423, 0.705904, 0.614256, 0.794525, 0.539166, 0.621489, 0.547419, 0.584198, 0.373561, 0.549009, 0.591812, 0.506334, 0.462293, 0.492157, 0.271897, 0.596217, 0.497406, 0.509365, 0.510845, 0.465080, 0.569216, 0.306425, 0.401745, 0.508481, 0.389430, 0.445954, 0.210412, 0.397975, 0.462106, 0.120381, 0.311424, 0.200232, 0.213058, 0.365859, 0.406067, 0.266532, 0.225964, 0.448491, 0.250007, 0.287008, 0.204457, 0.280261, 0.097567, 0.212174, 0.111659, 0.171260, 0.249548, 0.366685, 0.115686, 0.204338, 0.095414, 0.140158, 0.399552, 0.129494, 0.041723, 0.191687, 0.173983, -0.080636
	DCFS -0.002455, 0.006768, 0.016214, 0.042078, -0.034445, 0.054937, 0.048668, -0.092664, 0.019186, -0.140589, 0.044091, -0.069773, 0.013628, -0.167701, -0.000141, -0.115919, -0.105513, -0.135368, -0.051007, 0.052217, -0.048356, -0.144918, -0.232503, -0.159208, -0.277224, -0.196030, -0.102901, -0.215652, -0.097397, -0.276243, -0.192401, -0.212856, -0.395555, -0.317384, -0.174333, -0.398205, -0.309323, -0.400966, -0.411004, -0.414681, -0.249671, -0.340745, -0.313370, -0.415650, -0.381905, -0.295017, -0.516699, -0.472999, -0.304248, -0.245980, -0.510817, -0.389603, -0.567208, -0.385696, -0.447053, -0.570599, -0.427131, -0.360337, -0.511258, -0.594022, -0.624566, -0.645517, -0.489991, -0.504351, -0.625049, -0.479634, -0.669886, -0.422825, -0.792511, -0.615235, -0.520586, -0.602690, -0.606951, -0.670589, -0.431028, -0.681342, -0.573199, -0.529499, -0.692162, -0.520817, -0.628720, -0.829253, -0.734894, -0.709512, -0.638354, -0.637961, -0.646539, -0.867888, -0.882634, -0.602304, -0.661234, -0.709010, -0.602184, -0.578606, -0.846953
	DCFS -0.771878, -0.739229, -0.776489, -0.774355, -0.781251, -0.697961, -1.087360, -0.818866, -0.849028, -0.876480, -0.820750, -0.787188, -0.874478, -0.849460, -1.021470, -0.973026, -0.903961, -0.888614, -0.983132, -0.999270, -0.824598, -0.877001, -0.757299, -0.844832, -1.020295, -1.129825, -0.898189, -0.938880, -0.864672, -0.832601, -0.947794, -0.916582, -1.007188, -0.818422, -0.936767, -0.998524, -0.994227, -0.769775, -0.903832, -0.915514, -0.926700, -0.917698, -0.918548, -0.967681, -0.799608, -0.997062, -1.059717, -1.018805, -0.861293, -0.903398, -0.866217, -1.006593, -0.971750, -1.169842, -0.895320, -1.001998, -0.986916, -0.892342, -1.022100, -0.952651, -1.068393, -1.050565, -0.943888, -1.019805, -1.155667, -1.053262, -0.708798, -0.950641, -0.978487, -1.040873, -1.181790, -0.986059, -1.307189, -1.091205, -1.143853, -1.019045, -0.889628, -0.944570, -1.039606, -1.156869, -1.046409, -1.110427, -0.947598, -1.047687, -0.845931, -0.997724, -1.136155, -0.974529, -0.908802, -1.028120, -0.914642, -0.940278, -0.933458, -1.002156
	DCFS -0.979766, -0.977832, -0.974561, -1.104498, -0.957866, -1.072103, -0.937190, -1.063458, -0.807272, -0.858092, -0.990655, -0.726727, -0.815612, -0.840475, -1.129264, -1.044646, -0.984289, -0.942728, -1.036922, -0.835976, -0.949382, -0.909185, -0.942950, -0.892281, -0.741507, -1.056955, -0.965150, -0.911104, -0.803461, -0.956079, -0.901000, -0.772839, -0.960563, -0.727507, -0.792464, -0.764023, -0.816753, -0.909178, -0.963473, -0.782189, -0.719899, -0.712883, -0.787677, -0.607951, -0.747876, -0.800215, -0.742857, -0.808362, -0.671324, -0.577640, -0.622471, -0.694746, -0.848672, -0.769207, -0.658114, -0.698887, -0.545722, -0.610551, -0.812609, -0.598478, -0.587385, -0.471581, -0.630458, -0.604965, -0.781020, -0.661664, -0.757074, -0.614544, -0.669663, -0.712432, -0.575924, -0.651339, -0.432227, -0.327947, -0.527045, -0.505656, -0.721235, -0.367126, -0.570662, -0.460552, -0.511085, -0.447862, -0.482073, -0.451533, -0.562130, -0.433531, -0.586669, -0.439676, -0.401505, -0.438511, -0.368957, -0.303851, -0.542532, -0.519413
	DCFS -0.426561, -0.312433, -0.311318, -0.372022, -0.269044, -0.109711, -0.415995, -0.383170, -0.472003, -0.266054, -0.190321, -0.297308, -0.141484, -0.263952, -0.206707, -0.119949, -0.152783, -0.170222, -0.049338, -0.273568, -0.218796, -0.132842, -0.188291, -0.199082, -0.060754, -0.173638, -0.073438, -0.121339, -0.136631, -0.037364, -0.044010, -0.260514, -0.087066, -0.151033, -0.021990, -0.094930, -0.014601, 0.122172, 0.176990, 0.096668, -0.133000, 0.183526, 0.154305, 0.082128, 0.309812, 0.030644, 0.266074, -0.013986, 0.066888, -0.000706, 0.182736, 0.172783, 0.081166, 0.015476, -0.103719, 0.080533, 0.042277, 0.261655, 0.313086, 0.014904, 0.050136, 0.186713, 0.339050, 0.379321, 0.410707, 0.233654, 0.217137, 0.189100, 0.357117, 0.388021, 0.168013, 0.316500, 0.286773, 0.409507, 0.473033, 0.487912, 0.425989, 0.344611, 0.347920, 0.287416, 0.530408, 0.333220, 0.237361, 0.358390, 0.595150, 0.484235, 0.431137, 0.497058, 0.524239, 0.537908, 0.484754, 0.739844, 0.625792, 0.280790, 0.441806, 0.542108, 0.603429, 0.533536, 0.565667
	DCFS 0.545948, 0.454931, 0.807614, 0.583035, 0.843063, 0.529464, 0.600595, 0.503193, 0.556218, 0.739619, 0.732563, 0.576945, 0.687028, 0.635265, 0.769374, 0.616912, 0.856589, 0.553073, 0.744208, 0.570785, 0.656404, 0.628217, 0.897395, 0.732922, 0.788343, 0.618080, 0.672698, 0.809821, 0.771031, 0.853608, 0.909379, 0.707773, 0.708623, 0.857168, 0.805279, 0.824371, 0.740162, 0.739415, 0.884425, 1.001148, 0.815785, 0.966472, 0.997759, 0.973034, 1.022435, 0.911545, 0.719028, 0.802772, 0.874302, 0.749083, 0.846288, 1.030518, 0.829251, 0.931756, 0.943655, 1.104028, 0.898206, 0.991717, 0.807745, 0.846773, 0.970655, 0.993855, 0.897528, 0.975449, 0.883838, 1.011577, 0.886863, 0.985077, 0.997943, 1.108647, 0.856974, 0.991760, 0.800110, 0.987704, 1.055624, 0.872550, 0.960730, 1.105985, 1.150482, 0.986790, 1.136751, 0.973797, 0.903644, 0.981940, 0.819251, 0.911973, 0.933465, 1.134318, 1.019520, 1.202368, 1.043057, 1.034397, 0.977049, 0.910558, 1.016849, 0.962378, 0.844257, 1.051365, 0.983162, 1.025551, 0.946902, 1.066971, 0.937968
	DCFS 1.032691, 1.063122, 0.982859, 0.902348, 1.017141, 1.099789, 0.772960, 1.000759, 1.001968, 1.021246, 0.946681, 1.016567, 0.925441, 1.004383, 1.091572, 0.965489, 0.864355, 0.959553, 1.010816, 0.937803, 1.128276, 0.915258, 0.913998, 1.116507, 0.980344, 1.000856, 1.077439, 0.875212, 0.918722, 1.020709, 0.850468, 0.950302, 0.806263, 0.969632, 1.138271, 0.894365, 0.866214, 1.068280, 0.804013, 0.855705, 0.885547, 0.944899, 0.770580, 0.777904, 0.900235, 0.845899, 0.692148, 0.682239, 0.676151, 0.846024, 0.883951, 0.899856, 0.721378, 0.620839, 0.875409, 0.801946, 0.802693, 0.757878, 0.761271, 0.976463, 0.764685, 0.824853, 0.550271, 0.612217, 0.676255, 0.762503, 0.777313, 0.985376, 0.551840, 0.547862, 0.685590, 0.619726, 0.595699, 0.787666, 0.642459, 0.724661, 0.690767, 0.502904, 0.732036, 0.622234, 0.573999, 0.617182, 0.664058, 0.727380, 0.619192, 0.428553, 0.676431, 0.530269, 0.761298, 0.629583, 0.625740, 0.517912, 0.459323, 0.462238, 0.397093, 0.435860, 0.627843, 0.441195, 0.486384, 0.639352, 0.424965, 0.445867, 0.526275
	DCFS 0.468165, 0.485523, 0.392422, 0.403520, 0.574109, 0.605136, 0.390379, 0.307197, 0.366271, 0.308323, 0.284887, 0.149944, 0.182978, 0.325622, 0.254389, 0.441834, 0.114961, 0.197658, 0.331220, 0.318234, 0.217674, 0.151687, 0.005216, 0.185045, 0.269844, 0.201034, 0.066430, 0.289124, 0.257425, 0.154853, 0.172254, 0.161526, 0.162111, 0.211555, 0.088422, 0.273425, -0.009888, 0.062441, 0.004589, 0.008134, -0.100461, 0.171324, 0.052391, 0.022079, 0.034397, -0.056298, -0.032217, 0.084515, -0.021878, 0.025883, 0.030322, 0.057609, -0.108691, -0.139702, -0.037206, -0.179402, 0.022481, -0.212362, -0.005806, 0.037750, -0.263660, -0.026501, -0.102701, -0.152906, -0.198123, -0.202735, -0.105692, -0.268455, -0.274544, 0.085709, -0.073909, -0.134046, -0.156659, -0.264661, -0.445716, -0.300473, -0.214808, -0.233349, -0.149584, -0.197303, -0.283596, -0.393578, -0.376066, -0.306095, -0.413580, -0.420241, -0.239576, -0.407601, -0.561517, -0.631798, -0.397092, -0.478564, -0.479404, -0.462583, -0.453813, -0.568315, -0.477762, -0.703568
	DCFS -0.586115, -0.540822, -0.482293, -0.490827, -0.400456, -0.367371, -0.499525, -0.635042, -0.507367, -0.443676, -0.464315, -0.575029, -0.738917, -0.730247, -0.645946, -0.565172, -0.684011, -0.789728, -0.508457, -0.693459, -0.555748, -0.619591, -0.801278, -0.672621, -0.618064, -0.761270, -0.769306, -0.674950, -0.817804, -0.789154, -0.595297, -0.769081, -0.695980, -0.797000, -0.909083, -0.809084, -0.823839, -0.753948, -0.830653, -0.685523, -0.998279, -0.759855, -0.857953, -0.714167, -0.809618, -0.658990, -0.868678, -0.865146, -0.791355, -0.738933, -0.970391, -1.119834, -0.879017, -0.840145, -0.717691, -0.900966, -0.870514, -0.817360, -0.950878, -0.994964, -0.921001, -1.080028, -0.856073, -1.014437, -1.070445, -0.913022, -0.874936, -0.950036, -0.877207, -0.858501, -0.869260, -0.961673, -0.917687, -1.002694, -0.799649, -1.025672, -1.071891, -1.022118, -1.031695, -1.092797, -1.095213, -0.797064, -0.965667, -1.067042, -0.870143, -1.074789, -0.827512, -1.011993, -1.061492, -0.854294, -1.043693, -0.988528, -0.918108, -0.963088
	DCFS -1.057234, -1.205061, -1.073647, -0.928091, -0.889504, -0.973287, -0.905745, -0.966680, -0.854142, -0.849760, -1.058210, -0.997968, -0.939863, -0.871563, -0.984937, -1.050891, -1.002004, -1.131817, -1.051145, -1.083185, -0.978208, -1.019958, -0.879986, -0.973817, -1.105623, -0.947052, -1.089110, -0.967368, -0.741987, -1.028591, -0.962875, -0.866578, -1.080687, -0.851666, -1.015005, -0.889378, -1.023656, -0.794737, -0.845923, -1.031194, -0.776902, -0.984913, -0.873358, -0.933186, -0.948321, -0.741499, -1.095131, -1.089591, -0.853141, -1.147074, -0.844848, -0.792516, -1.067969, -0.918600, -0.971152, -0.813844, -0.966185, -0.818134, -0.970509, -0.781815, -0.847661, -0.998830, -0.990562, -0.864911, -1.063417, -0.768987, -0.707779, -0.784068, -0.959643, -0.815456, -0.610258, -0.702702, -0.768249, -0.729693, -0.676958, -0.786205, -0.810097, -0.820375, -0.771118, -0.804293, -0.851194, -0.861387, -0.701197, -0.811981, -0.842998, -0.879349, -0.872834, -0.660004, -0.758185, -0.652918, -0.586254, -0.727378, -0.620980, -0.396691
	DCFS -0.712515, -0.626842, -0.694708, -0.544957, -0.689099, -0.683153, -0.567436, -0.628142, -0.686601, -0.826927, -0.743080, -0.648346, -0.507377, -0.508185, -0.515270, -0.458861, -0.573279, -0.506827, -0.542758, -0.483185, -0.538140, -0.407742, -0.658567, -0.257535, -0.512032, -0.599448, -0.581923, -0.462684, -0.319467, -0.535024, -0.389792, -0.505230, -0.318102, -0.459611, -0.475030, -0.168432, -0.348881, -0.236727, -0.262480, -0.327848, -0.204146, -0.341471, -0.266002, -0.466497, -0.423797, -0.215528, -0.317883, -0.313883, -0.224808, -0.516477, -0.279916, -0.342163, -0.177209, -0.211631, -0.358109, -0.205046, -0.207872, -0.123024, -0.228174, -0.290683, -0.153406, -0.085819, -0.089945, -0.050049, -0.199845, -0.181413, -0.153176, -0.014571, -0.037958, -0.021243, 0.055396, 0.031403, -0.043784, -0.000209, 0.216438
	END
			

			