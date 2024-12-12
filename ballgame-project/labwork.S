  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
 
  # Please add your own code below this line
	.text
.global hexasc
	hexasc:
	li $t2, 10  # Upper bound
	andi $t1, $a0, 0x000F
	
	sgt $t3, $t2, $t1  # om 10 > a0
	beq $t3, 1, adder48  # If false, branch to exit
	nop
	
	addi $v0, $t1, 55
	jr $ra
	nop

.global adder48
	adder48:
	addi $v0, $t1, 48
	jr $ra
	nop
	
	#DELAY
.global delay
	delay:
	PUSH $ra
	move $t2, $a0 # put ms in t2
	
	.global oloop
	oloop: # outer loop
	ble  $t2, $zero, exit_delay
	nop
	addi $t2, $t2, -1
	li $t0, 0 # dont change
	li $t1, 3800 # const to be changed
		.global iloop
		iloop: # inner loop
		bge   $t0, $t1, oloop # jump to oloop if i >= to 4711
		nop
		addi $t0, $t0, 1
		j iloop
		nop
	j oloop
	nop
	
.global exit_delay
	exit_delay:
	POP $ra
	jr $ra
	nop
  
.global time2string
	time2string:
	PUSH $s0
	PUSH $s1
	PUSH $ra
	move $s1, $a1
	move $s0, $a0
	
	# first digit
	andi $t1, $s1, 0xF000
	srl $a0, $t1, 12
	jal hexasc
	nop
	
	sb $v0, 0($s0)
	
	#second digit
	andi $t1, $s1, 0x0F00
	srl $a0, $t1, 8
	jal hexasc
	nop
	
	sb $v0, 1($s0)
	
	# colon : 58 (3x16+10)
	li $t1, 0x3a
	sb $t1, 2($s0)
	
	
	#third digit
	andi $t1, $s1, 0x00F0
	srl $a0, $t1, 4
	jal hexasc
	nop
	
	sb $v0, 3($s0)
	
	#fourth digit
	andi $t1, $s1, 0x000F
	srl $a0, $t1, 0
	jal hexasc
	nop
	
	sb $v0, 4($s0)
	
	# null byte
	li $t1, 0x00
	sb $t1, 5($s0)
	
	#restore registers
	POP $ra
	POP $s1
	POP $s0
	# exit
	jr $ra
	nop

.global enable_interrupt
    enable_interrupt:
        mfc0    $t0, $12        
        ori     $t0, $t0, 0x1   
        mtc0    $t0, $12        
        nop                     
        jr      $ra             
        nop                     
