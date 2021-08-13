extends Node

signal language_changed(language)

var language: String = TranslationServer.get_locale().substr(0, 2) setget set_language
var is_accessibility_mode: bool


func _unhandled_input(event: InputEvent) -> void:
	
	if event.is_action_pressed("full_screen"):
		OS.window_fullscreen = !OS.window_fullscreen


func set_language(value: String) -> void:
	
	TranslationServer.set_locale(value)
	language = value
	emit_signal("language_changed", value)
