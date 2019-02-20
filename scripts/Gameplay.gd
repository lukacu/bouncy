extends Node

var size = Vector2(640, 480);

onready var camera = preload("res://camera.gdns").new()

var active = false
var player
var camera_size
var texture_size
var image = Image.new();

signal camera_ready
signal camera_frame

signal game_start
signal force

func _ready():
	camera.set_default(0)
	camera.open();
	camera.flip(true, false);
	camera_size = Vector2(camera.get_width(), camera.get_height());
	texture_size = max(camera_size.x, camera_size.y);
	
	emit_signal("camera_ready", camera_size);

	player = get_node("/root/root/game/player");

	$animation.play("greeting_blink");

func _process(delta):
	var buffer = camera.get_image();
	if not buffer:
		return;
	image.create_from_data(texture_size, texture_size, false, Image.FORMAT_RGB8, buffer);
	emit_signal("camera_frame", image)
	
	if !active:
		var face = camera.detect_face()
		if face and not active:
			var position = face.position + face.size / 2;
			if (position - size / 2).length() < 40:
				active = true
				emit_signal("game_start", position)
				$animation.play("game_start")
	else:
		var region = Rect2(player.get("position"), Vector2(70, 70))
		var force = camera.compute_flow(region)
		emit_signal("force", Vector2(clamp(force.x, -100, 100), clamp(force.y, -100, 100)))

