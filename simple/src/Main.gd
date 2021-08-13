extends Node

const PieceScn: PackedScene = preload("res://src/objects/Piece.tscn")
const APARTMENT_MATERIAL: Material = preload("res://assets/materials/Apartment.tres")
const PARK_MATERIAL: Material = preload("res://assets/materials/Park.tres")
const ROOF_MATERIAL: Material = preload("res://assets/materials/Roof.tres")
const APARTMENT_TEXTURE: Texture = preload("res://assets/textures/elevator.png")
const PARK_TEXTURE: Texture = preload("res://assets/textures/park-bench.png")
const ROOF_TEXTURE: Texture = preload("res://assets/textures/private.png")
const SOUND_BANK := preload("res://assets/sfxs/sound_bank.tres")

const MATERIALS = [
	APARTMENT_MATERIAL, PARK_MATERIAL, ROOF_MATERIAL,
]
const TEXTURES = [
	APARTMENT_TEXTURE, PARK_TEXTURE, ROOF_TEXTURE,
]
const CAMERA_ANIMATIONS := PoolStringArray([
	"GoEast",
	"GoSouth",
	"GoWest",
	"GoNorth",
])
const PLAYERS := PoolStringArray([
	"NORTH",
	"EAST",
	"SOUTH",
	"WEAST",
])

enum PieceType {
	APARTMENT = 0,
	PARK = 1,
	ROOF = 2,
} # Os valores dese `enum` são equivalentes ao que foram usados na biblioteca,
# Não exportei esses valores a partir da biblioteca em si para evitar a fadiga.
# (assim como fiz com muitas coisas desse projeto). :v

export var pitch_shift_min: float = 0.5
export var pitch_shift_max: float = 1.5

var replaying: bool = false
var current_x: int = 0
var current_y: int = 0
var current_player: int = 2
var current_piece_type: int = PieceType.APARTMENT
var data: String

onready var simple := preload("res://bin/simple.gdns").new()
onready var tween: Tween = $Tween
onready var pieces: Spatial = $Spatial/Table/Pieces
onready var current_piece: MeshInstance = $Spatial/Table/Pieces/Apartment
onready var camera_animation_player: AnimationPlayer = \
		$Spatial/HBoxContainer/ViewportContainer/Viewport/Camera/AnimationPlayer
onready var animation_tree: AnimationTree = \
		$Spatial/HBoxContainer/ViewportContainer/Viewport/Camera/AnimationPlayer/AnimationTree
onready var points_label: Label = \
		$CanvasLayer/HBoxContainer/PlayerPoints/HBoxContainer/PointsLabel
onready var player_label: Label = \
		$CanvasLayer/HBoxContainer/CurrentPlayer/HBoxContainer/PlayerLabel
onready var camera: Camera = $Spatial/HBoxContainer/ViewportContainer/Viewport/Camera
onready var game_over_dialog: ConfirmationDialog = $CanvasLayer/GameOverDialog
onready var game_start_dialog: AcceptDialog = $CanvasLayer/GameStartDialog
onready var pieces_displayer: Spatial = \
		$ViewportCanvasLayer/ViewportContainer/Viewport/PiecesDisplayer
onready var audio_stream_player: AudioStreamPlayer = $Spatial/AudioStreamPlayer
onready var game_start_dialog_label: RichTextLabel = $CanvasLayer/GameStartDialog/RichTextLabel


#func _ready() -> void:
#	data = "World from GDScript!"
#	yield(get_tree().create_timer(1), "timeout")
#	$CenterContainer/VBoxContainer/GDScriptLabel.text = "Hello %s" % get_data()


# Instancia as peças iniciais.
func _ready() -> void:
	var apartments: int
	set_process_unhandled_input(false)
	animation_tree.active = false
	
	if Game.is_accessibility_mode:
		$Spatial/Table/Pieces/Park/Sprite3D.visible = true
		$Spatial/Table/Pieces/Apartment/Sprite3D.visible = true
		game_start_dialog_label.bbcode_text = tr("GAME_START_DIALOG_ACCM")
		
	else:
		# Eu sei que essa não é a forma mais elegante de se fazer isso, mas como meu budget de tempo
		# está se esgotando, vai assim mesmo.
		# Em outro caso, eu faria o processamento da string apropriadamente. #POG :V
		game_start_dialog_label.bbcode_text = tr("GAME_START_DIALOG")
	
	var block: MeshInstance #= instance_block(4, 4, 0, PieceType.APARTMENT)
	#block.get_node("Sprite3D").texture = TEXTURES[PieceType.APARTMENT]
	current_piece.get_node("Sprite3D").texture = TEXTURES[PieceType.APARTMENT]
	
	for i in range(1, 4):
		
		for j in [1, 3]:
			apartments = simple.get_size_at(i, j)
			
			if (apartments == -1):
				continue
			
			for k in apartments:
				block = instance_block(i, j, k)
				block.get_node("Sprite3D").visible = Game.is_accessibility_mode
	
	for i in [1, 3]:
		apartments = simple.get_size_at(i, 2)
		
		for k in apartments:
			block = instance_block(i, 2, k)
			block.get_node("Sprite3D").visible = Game.is_accessibility_mode
	
	game_start_dialog.popup()


func _unhandled_input(event: InputEvent) -> void:
	
	if event.is_action_pressed("ui_up") and current_y > 0:
		current_y -= 1
		update_current_piece()
		get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_down") and current_y < 4:
		current_y += 1
		update_current_piece()
		get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_left") and current_x > 0:
		current_x -= 1
		update_current_piece()
		get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_right") and current_x < 4:
		current_x += 1
		update_current_piece()
		get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_focus_prev"):
		current_piece_type = (current_piece_type + 1) % 3
		current_piece.set_surface_material(0, MATERIALS[current_piece_type])
		current_piece.get_node("Sprite3D").texture = TEXTURES[current_piece_type]
		get_tree().set_input_as_handled()
		
	elif event.is_action_pressed("ui_focus_next"):
		current_piece_type = (3 + current_piece_type - 1) % 3
		
		# TODO -> Impedir peças usadas de serem selecionadas.
		
		current_piece.set_surface_material(0, MATERIALS[current_piece_type])
		current_piece.get_node("Sprite3D").texture = TEXTURES[current_piece_type]
		get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_accept"):
		
		if simple.move_piece(current_x, current_y, current_piece_type, current_player) == 1:
			
			if simple.can_continue_game():
				next_player()
			else:
				game_over()
			
		else:
			camera.tilt()
			play_random_sound(SOUND_BANK.TILTS)


func _on_GameOverDialog_confirmed() -> void:
	replaying = true
	game_over_dialog.hide()
	
	if get_tree().reload_current_scene() != OK:
		push_warning("Wouldn't able to reload current scene.")


func _on_GameOverDialog_popup_hide() -> void:
	
	if not replaying:
		get_tree().quit()


func _on_GameStartDialog_popup_hide() -> void:
	set_process_unhandled_input(true)


# Cria uma instância de uma peça na posição determinada.
func instance_block(table_x: int, table_y: int, stack_size: int) -> MeshInstance:
	var piece: MeshInstance = PieceScn.instance()
	
	piece.translation = board_coords_to_position(table_x, table_y, stack_size)
	pieces.add_child(piece)
	
	return piece


# Converte as coordenadas do tabuleiro para uma posição no espaço.
func board_coords_to_position(table_x: int, table_y: int, stack_size: int) -> Vector3:
	table_x += 1
	table_y += 1
	
	if stack_size < 0:
		stack_size = 0
	
	return Vector3(
		(float(table_x) * 2.0) - 6.0, float(stack_size) * 1.075 + .5, (float(table_y) * 2.0) - 6.0)


# Sincroniza a posição da peça do jogador atual com as coordenas em cache e toca um efeito sonoro.
func update_current_piece() -> void:
#	current_piece.translation = board_coords_to_position(
#			current_x, current_y, simple.get_size_at(current_x, current_y))
	play_random_sound(SOUND_BANK.SWOOSHS)
	
	if not (
		tween.stop(current_piece, ":translation") and \
		tween.interpolate_property(current_piece, "translation", current_piece.translation,
			board_coords_to_position(
				current_x, current_y, simple.get_size_at(current_x, current_y)
			), .3, Tween.TRANS_SINE) and \
		tween.start()
	):
		push_warning("Wouldn't able to interpolate property :translation from %s." % current_piece)


# Faz a seleção do próximo jogador.
func next_player() -> void:
	current_x = 0
	current_y = 0
	current_piece_type = PieceType.APARTMENT
	current_piece = instance_block(current_x, current_y, simple.get_size_at(current_x, current_y))
	current_piece.get_node("Sprite3D").visible = Game.is_accessibility_mode
	camera_animation_player.play(CAMERA_ANIMATIONS[current_player])
	current_player = (current_player + 1) % 4


# Atualiza a ui com dados relacionados ao jogador.
func update_player(from: int) -> void:
	points_label.text = str(simple.get_player_points(from))
	player_label.text = tr(PLAYERS[from])
	pieces_displayer.set_pieces_visibility(
		simple.get_pieces_count(PieceType.APARTMENT, from),
		simple.get_pieces_count(PieceType.PARK, from),
		simple.get_pieces_count(PieceType.ROOF, from))


# Apresenta o popup de fim de jogo, e toca a animação final.
func game_over() -> void:
	var winner: int = simple.get_winner(0)
	
	game_over_dialog.window_title = "%s %s %s!" % [tr("PLAYER"), tr(PLAYERS[winner]), tr("WON")]
	game_over_dialog.dialog_text = \
		str(tr("POINTS"), " ", str(simple.get_player_points(winner)), ".\n", tr("PLAY_AGAIN"))
	game_over_dialog.popup()
	
	animation_tree.active = true
	animation_tree.tree_root.get_node("StateMachine")\
			.set_start_node(CAMERA_ANIMATIONS[current_player])
	animation_tree["parameters/StateMachine/playback"]\
			.travel(CAMERA_ANIMATIONS[(current_player + 1) % 4])
	
	set_process_unhandled_input(false)


# Reproduz um efeito sonoro aleatório de uma lista de sons.
func play_random_sound(sfx_list: Array) -> void:
	audio_stream_player.stream = sfx_list[rand_range(0, sfx_list.size())]
	audio_stream_player.pitch_scale = rand_range(pitch_shift_min, pitch_shift_max)
	audio_stream_player.play()

#func get_data() -> String:
#	return data


#func _on_Button_pressed() -> void:
#	$CenterContainer/VBoxContainer/GDNativeLabel.text = "Hello %s" % simple.get_data()
#	print(simple.test())
