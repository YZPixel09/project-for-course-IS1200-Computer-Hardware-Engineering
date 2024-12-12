  # timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0, 2
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
hexasc:
	li $t2, 10  # Upper bound
	andi $t1, $a0, 0x000F
	
	sgt $t3, $t2, $t1  # om 10 > a0
	beq $t3, 1, adder48  # If false, branch to exit
	nop
	
	addi $v0, $t1, 55
	jr $ra
	nop

adder48: 
	addi $v0, $t1, 48
	jr $ra
	nop

delay:
	PUSH ($ra)
	move $t2, $a0 # put ms in s0
	
	oloop: # outer loop
	ble $t2,$zero, exit_delay
	nop
	addi $t2, $t2, -1
	li $t0, 0 # dont change
	li $t1, 5000 # const to be changed
	iloop: # inner loop
		bge  $t0, $t1, oloop # jump to oloop if i >= to 4711
		nop
		addi $t0, $t0, 1
		j iloop
		nop
	j oloop
	nop
	
exit_delay:
	POP ($ra)
	jr $ra
	nop
	
time2string:
	PUSH ($s0)
	PUSH ($s1)
	PUSH ($ra)
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
	POP ($ra)
	POP ($s1)
	POP ($s0)
	# exit
	jr $ra
	nop
	
	
	
	
	 
    	
    	
    	
