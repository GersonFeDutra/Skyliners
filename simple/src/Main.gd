extends Node

const PieceScn: PackedScene = preload("res://src/objects/Piece.tscn")
const APARTMENT_MATERIAL: Material = preload("res://assets/materials/Apartment.tres")
const PARK_MATERIAL: Material = preload("res://assets/materials/Park.tres")
const ROOF_MATERIAL: Material = preload("res://assets/materials/Roof.tres")
const APARTMENT_TEXTURE: Texture = preload("res://assets/textures/elevator.png")
const PARK_TEXTURE: Texture = preload("res://assets/textures/park-bench.png")
const ROOF_TEXTURE: Texture = preload("res://assets/textures/private.png")
const SOUND_BANK := preload("res://assets/sfxs/sound_bank.tres")
const PLAYERS_N: int = 4

const MATERIALS = [
	APARTMENT_MATERIAL, PARK_MATERIAL, ROOF_MATERIAL,
]
const TEXTURES = [
	APARTMENT_TEXTURE, PARK_TEXTURE, ROOF_TEXTURE,
]
const CAMERA_ANIMATIONS := PoolStringArray([
	"go_north",
	"go_east",
	"go_south",
	"go_west",
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
onready var pieces: Spatial = $World/Table.get_pieces()
onready var current_piece: MeshInstance = $World/Table.get_apartment()
onready var camera: Camera = $World/SplitScreen.get_camera()
onready var camera_animation_player: AnimationPlayer = $AnimationController
onready var animation_tree: AnimationTree = $AnimationController/AnimationTree
onready var hud: Node = $HUD
onready var audio_stream_player: AudioStreamPlayer = $World/AudioStreamPlayer


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
		hud.game_start_label.bbcode_text = tr("GAME_START_DIALOG_ACCM")
		
	else:
		# Eu sei que essa não é a forma mais elegante de se fazer isso, mas como meu budget de tempo
		# está se esgotando, vai assim mesmo.
		# Em outro caso, eu faria o processamento da string apropriadamente. #POG :V
		hud.game_start_label.bbcode_text = tr("GAME_START_DIALOG")
	
	var block: MeshInstance #= instance_block(4, 4, 0, PieceType.APARTMENT)
	#block.get_node("Sprite3D").texture = TEXTURES[PieceType.APARTMENT]
	current_piece.get_node("Sprite3D").texture = TEXTURES[PieceType.APARTMENT]
	
	for i in range(1, 4):
		
		for j in [1, 3]:
			apartments = simple.get_size_at(i, j, current_player)
			
			if (apartments == -1):
				continue
			
			for k in apartments:
				block = instance_block(i, j, k)
				block.get_node("Sprite3D").visible = Game.is_accessibility_mode
	
	for i in [1, 3]:
		apartments = simple.get_size_at(i, 2, current_player)
		
		for k in apartments:
			block = instance_block(i, 2, k)
			block.get_node("Sprite3D").visible = Game.is_accessibility_mode
	
	hud.game_start_dialog.popup()


func _unhandled_input(event: InputEvent) -> void:
	
	if event.is_action_pressed("ui_up"):
		var coords_data: Dictionary = simple.get_next_rotated_coords(
					current_x, current_y, 0, -1, current_player)
		
		if coords_data.is_on_bounds:
			current_y -= 1
			
			update_current_piece()
			get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_down"):
		var coords_data: Dictionary = simple.get_next_rotated_coords(
				current_x, current_y, 0, +1, current_player)
		
		if coords_data.is_on_bounds:
			current_y += 1
			update_current_piece()
			get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_left"):
		var coords_data: Dictionary = simple.get_next_rotated_coords(
				current_x, current_y, -1, 0, current_player)
		
		if coords_data.is_on_bounds:
			current_x -= 1
			update_current_piece()
			get_tree().set_input_as_handled()
	
	if event.is_action_pressed("ui_right"):
		var coords_data: Dictionary = simple.get_next_rotated_coords(
				current_x, current_y, +1, 0, current_player)
		
		if coords_data.is_on_bounds:
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
	hud.game_over_dialog.hide()
	
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
	play_random_sound(SOUND_BANK.SWOOSHS)
	var coords_data: Dictionary = \
			simple.get_rotated_coords(current_x, current_y, current_player)
	
	if not (
		tween.stop(current_piece, ":translation") and \
		tween.interpolate_property(current_piece, "translation", current_piece.translation,
			board_coords_to_position(coords_data.x, coords_data.y, 
					simple.get_size_at(current_x, current_y, current_player)),
				.3, Tween.TRANS_SINE) and \
		tween.start()
	):
		push_warning("Wouldn't able to interpolate property :translation from %s." % current_piece)


# Faz a seleção do próximo jogador.
func next_player() -> void:
	var coords_data: Dictionary
	current_x = 0
	current_y = 0
	current_piece_type = PieceType.APARTMENT
	current_player = (current_player + 1) % PLAYERS_N
	
	coords_data = simple.get_rotated_coords(current_x, current_y, current_player)
	current_piece = instance_block(coords_data.x, coords_data.y, simple.get_size_at(
			current_x, current_y, current_player))
	current_piece.get_node("Sprite3D").visible = Game.is_accessibility_mode
	camera_animation_player.play(CAMERA_ANIMATIONS[current_player])


# Atualiza a ui com dados relacionados ao jogador.
func update_player(from: int) -> void:
	hud.points_label.text = str(simple.get_player_points(from))
	hud.player_label.text = tr(PLAYERS[from])
	hud.pieces_displayer.set_pieces_visibility(
		simple.get_pieces_count(PieceType.APARTMENT, from),
		simple.get_pieces_count(PieceType.PARK, from),
		simple.get_pieces_count(PieceType.ROOF, from))


# Apresenta o popup de fim de jogo, e toca a animação final.
func game_over() -> void:
	var winner: int = simple.get_winner(0)
	
	hud.game_over_dialog.window_title = "%s %s %s!" % [tr("PLAYER"), tr(PLAYERS[winner]), tr("WON")]
	hud.game_over_dialog.dialog_text = \
		str(tr("POINTS"), " ", str(simple.get_player_points(winner)), ".\n", tr("PLAY_AGAIN"))
	hud.game_over_dialog.popup()
	
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
