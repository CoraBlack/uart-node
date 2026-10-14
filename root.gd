extends Node2D

@export var uart: UartDev

func _ready() -> void:
	print("Pressed a and send msg to uart device")
	uart.set_port("/dev/ttyUSB0")
	
func _process(delta: float) -> void:
	if Input.is_action_just_pressed("SendMsg"):
		if uart.avaliable():
			var a := "Key a is pressed\r\n"
			uart.write_data(a.to_ascii_buffer())
			print("A is sent")
		else:
			print("Uart is not avaliable")
