extends Spatial


onready var apartments_parent: Spatial = $Apartments
onready var roofs_parent: Spatial = $Roofs
onready var park_piece: MeshInstance = $Park


func _ready() -> void:
	
	if not Game.is_accessibility_mode:
		return
	
	for apartment in apartments_parent.get_children():
		apartment.get_node("Sprite3D").visible = true
	
	for roof in roofs_parent.get_children():
		roof.get_node("Sprite3D").visible = true
	
	park_piece.get_node("Sprite3D").visible = true


func set_pieces_visibility(apartments: int, parks: int, roofs: int) -> void:
	
	for apartment in apartments:
		apartments_parent.get_node("Apartment%s" % str(apartment)).show()
	
	for apartment in range(apartments, 12):
		apartments_parent.get_node("Apartment%s" % str(apartment)).hide()
	
	for roof in roofs:
		roofs_parent.get_node("Roof%s" % str(roof)).show()
	
	for roof in range(roofs, 2):
		roofs_parent.get_node("Roof%s" % str(roof)).hide()
	
	if parks == 0:
		park_piece.hide()
	else:
		park_piece.show()
