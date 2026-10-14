extends Node2D

@export var uart: UartDev

func _ready() -> void:
	uart.set_port("/dev/ttyUSB0")
	
func _process(delta: float) -> void:
	if Input.is_key_pressed(KEY_A):
		if uart.avaliable():
			var a := "A"
			uart.write_data(a.to_ascii_buffer())
			print("A is sent")
		else:
			print("Uart is not avaliable")
