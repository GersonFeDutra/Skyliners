extends AcceptDialog

onready var animation_player: AnimationPlayer = $CanvasLayer/BlurRect/AnimationPlayer

func _on_GameStartDialog_about_to_show() -> void:
	animation_player.play("BlurIn")


func _on_GameStartDialog_popup_hide() -> void:
	animation_player.play("BlurOut")
