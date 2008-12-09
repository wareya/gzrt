#include "regs.h"

# Binary loader
loadbin:

	# Stack pointer
	la		$sp, 0x803FFFF0
	
	# Note! This stack pointer address WILL get overridden
	# by the original game at some point. This is just the
	# default setting so that the binary doesn't fuck up.
	
	# ROM pointer
	lui		$v0, 0xb065
	ori     $v0, $v0, 0x4321
	
	# DMA registers
	lui     $v1, 0xa460
	
	# Store ROM pointer
	sw		$v0, 4($v1)
	
	# RAM target
	lui		$a0, 0x1234
	ori		$a0, $a0, 0x5678
	
	# Store RAM target 
	sw		$a0, 0($v1)
	
	# Transfer size
	lui		$v0, 0
	ori		$v0, 0x4660
	
	# Store it
	sw		$v0, 12($v1)
	
	# Wait until transfer complete
wait:
	lw		$v0, 16($v1)
	andi	$v0, $v0, 1
	bnez	$v0, wait
	
	# Call it
	li		$v0, 0x80000000
	or		$v0, $a0, $v0
	jr		$v0

