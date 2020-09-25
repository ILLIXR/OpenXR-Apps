extends WorldEnvironment

func _ready():
	var interface = ARVRServer.find_interface("OpenXR")
	if interface and interface.initialize():
		get_viewport().arvr = true
		
		# swap this around if PR # 19724 has not been applied
		get_viewport().hdr = false
		# get_viewport().rgba8_out = true
		
		# for some reason the inspector won't let me set this up
		# $OVRFirstPerson/Right_Hand.connect("button_pressed", $OVRFirstPerson/HUD_Anchor/Settings_VR, "_on_Right_Hand_button_pressed")

func _process(delta):
	# Test for escape to close application, space to reset our reference frame
	if (Input.is_key_pressed(KEY_ESCAPE)):
		get_tree().quit()
	elif (Input.is_key_pressed(KEY_SPACE)):
		# Calling center_on_hmd will cause the ARVRServer to adjust all tracking data so the player is centered on the origin point looking forward
		ARVRServer.center_on_hmd(true, true)

	# We minipulate our origin point to move around. Note that with roomscale tracking a little more then this is needed
	# because we'll rotate around our origin point, not around our player. But that is a subject for another day.
	if (Input.is_key_pressed(KEY_LEFT)):
		$ARVROrigin.rotation.y += delta
	elif (Input.is_key_pressed(KEY_RIGHT)):
		$ARVROrigin.rotation.y -= delta

	if (Input.is_key_pressed(KEY_UP)):
		$ARVROrigin.translation -= $ARVROrigin.transform.basis.z * delta;
	elif (Input.is_key_pressed(KEY_DOWN)):
		$ARVROrigin.translation += $ARVROrigin.transform.basis.z * delta;
