
# We've hooked into a function call located at 0x800027A4. It's called
# frequently enough to suit our needs.

#define	replaced_func	0x00002964

	.set	noat

load:
	
	# Store the return address
	la		$at, tmpstor
	sw		$ra, 0($at)
	
	# Call the function we replaced
	jal		replaced_func
	
	# Grab some stack space...
	addi	$sp, -4
	
	# Preserve return value
	sw		$2, 0($sp)
	
	# Initialize data pointer
	la		$gp, _gp
	
	# Call our function
	jal		n64start
	
	# Restore stuff
	la		$at, tmpstor
	lw		$ra, 0($at)
	lw		$2, 0($sp)
	addi	$sp, 4
	
	# Return
	jr		$ra
	
tmpstor:
	.word 0x00000000
