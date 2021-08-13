extends Control


func _ready() -> void:
	$VBoxContainer/SkipButton.grab_focus()


func _on_CheckButton_toggled(button_pressed: bool) -> void:
	Game.is_accessibility_mode = button_pressed


func _on_SkipButton_pressed() -> void:
	$AnimationPlayer.play("fade")


func play() -> void:
	
	if get_tree().change_scene("res://src/Main.tscn") != OK:
		push_warning("Wouldn't able to change scene.")
