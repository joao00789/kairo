extends CharacterBody2D

@export var speed := 320.0
@onready var joystick: TouchJoystick2D = $"../CanvasLayer/TouchJoystick2D"

func _physics_process(_delta: float) -> void:
	# Usa o joystick na tela; no PC cai para o teclado (WASD).
	var dir := joystick.get_vector()
	if dir == Vector2.ZERO:
		dir = Input.get_vector("move_left", "move_right", "move_up", "move_down")
	velocity = dir * speed
	move_and_slide()

func _draw() -> void:
	draw_circle(Vector2.ZERO, 24.0, Color.CORNFLOWER_BLUE)
