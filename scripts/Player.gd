extends RigidBody2D

var reposition = null
var eye

var damping = 0.3

func _ready():
	eye = get_node("player_eye")
	set("visible", false)

func _integrate_forces(state):
	if reposition:
		set("visible", true)
		state.transform = Transform2D(0, reposition)
		reposition = null

func _on_root_game_start(position):
	reposition = position
	
func _on_root_force(force):
	apply_impulse(Vector2(), force)
	eye.set("position", (1 - damping) * eye.get("position") + damping * force.normalized() * 3)

#func _physics_process(delta):
#	var collision = move_and_collide(velocity * delta)
#	if collision:
#		velocity = velocity.bounce(collision.normal)
#		if collision.collider.has_method("hit"):
#			collision.collider.hit()