-- Huge object count example.
-- This sample demonstrates:
--     - Creating a scene with 250 x 250 simple objects;
--     - Competing with http://yosoygames.com.ar/wp/2013/07/ogre-2-0-is-up-to-3x-faster/ :);
--     - Allowing examination of performance hotspots in the rendering code;

require "LuaScripts/Utilities/Sample"

local scene_ = nil
local cameraNode = nil
local boxNodes = {}
local yaw = 0.0
local pitch = 0.0
local animate = false

local context = GetContext()

local cache = GetCache()
local input = GetInput()
local renderer = GetRenderer()
local ui = GetUI()

function Start()
    -- Execute the common startup for samples
    SampleStart()

    -- Create the scene content
    CreateScene()

    -- Create the UI content
    CreateInstructions()

    -- Setup the viewport for displaying the scene
    SetupViewport()

    -- Hook up to the frame update events
    SubscribeToEvents()
end

function Stop()
end

function CreateScene()
    scene_ = Scene(context)

    -- Create the Octree component to the scene so that drawable objects can be rendered. Use default volume
    -- (-1000, -1000, -1000) to (1000, 1000, 1000)
    scene_:CreateComponent("Octree")

    -- Create a Zone for ambient light & fog control
    local zoneNode = scene_:CreateChild("Zone")
    local zone = zoneNode:CreateComponent("Zone")
    zone.boundingBox = BoundingBox(-1000.0, 1000.0)
    zone.fogColor = Color(0.2, 0.2, 0.2)
    zone.fogStart = 200.0
    zone.fogEnd = 300.0
    
    -- Create a directional light
    local lightNode = scene_:CreateChild("DirectionalLight")
    lightNode.direction = Vector3(0.5, -1.0, 0.5) -- The direction vector does not need to be normalized
    local light = lightNode:CreateComponent("Light")
    light.lightType = LIGHT_DIRECTIONAL
    light.color = Color(0.7, 0.35, 0.0)

    -- Create box StaticModels in the scene
    for y = -125, 125 do
        for x = -125, 125 do
            local boxNode = scene_:CreateChild("Box")
            boxNode.position = Vector3(x * 0.3, 0.0, y * 0.3)
            boxNode:SetScale(0.25)
            local boxObject = boxNode:CreateComponent("StaticModel")
            boxObject.model = cache:GetResource("Model", "Models/Box.mdl")
            table.insert(boxNodes, boxNode)
        end
    end

    -- Create the camera
    cameraNode = scene_:CreateChild("Camera")
    cameraNode.position = Vector3(0.0, 10.0, -100.0)
    local camera = cameraNode:CreateComponent("Camera")
    camera.farClip = 300.0
end

function CreateInstructions()
    -- Construct new Text object, set string to display and font to use
    local instructionText = ui.root:CreateChild("Text")
    instructionText:SetText("Use WASD keys and mouse to move\n"..
        "Space to toggle animation")
    instructionText:SetFont(cache:GetResource("Font", "Fonts/Anonymous Pro.ttf"), 15)
    -- The text has multiple rows. Center them in relation to each other
    instructionText.textAlignment = HA_CENTER
    
    -- Position the text relative to the screen center
    instructionText.horizontalAlignment = HA_CENTER
    instructionText.verticalAlignment= VA_CENTER
    instructionText:SetPosition(0, ui.root.height / 4)
end

function SetupViewport()
    -- Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    local viewport = Viewport:new(context, scene_, cameraNode:GetComponent("Camera"))
    renderer:SetViewport(0, viewport)
end

function SubscribeToEvents()
    -- Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent("Update", "HandleUpdate")
end

function MoveCamera(timeStep)
    -- Do not move if the UI has a focused element (the console)
    if ui.focusElement ~= nil then
        return
    end

    -- Movement speed as world units per second
    local MOVE_SPEED = 20.0
    -- Mouse sensitivity as degrees per pixel
    local MOUSE_SENSITIVITY = 0.1

    -- Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    local mouseMove = input.mouseMove
    yaw = yaw + MOUSE_SENSITIVITY * mouseMove.x
    pitch = pitch + MOUSE_SENSITIVITY * mouseMove.y
    pitch = Clamp(pitch, -90.0, 90.0)

    -- Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode.rotation = Quaternion(pitch, yaw, 0.0)

    -- Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if input:GetKeyDown(KEY_W) then
        cameraNode:TranslateRelative(Vector3(0.0, 0.0, 1.0) * MOVE_SPEED * timeStep)
    end
    if input:GetKeyDown(KEY_S) then
        cameraNode:TranslateRelative(Vector3(0.0, 0.0, -1.0) * MOVE_SPEED * timeStep)
    end
    if input:GetKeyDown(KEY_A) then
        cameraNode:TranslateRelative(Vector3(-1.0, 0.0, 0.0) * MOVE_SPEED * timeStep)
    end
    if input:GetKeyDown(KEY_D) then
        cameraNode:TranslateRelative(Vector3(1.0, 0.0, 0.0) * MOVE_SPEED * timeStep)
    end
end

function AnimateObjects(timeStep)
    local ROTATE_SPEED = 15.0
    local delta = ROTATE_SPEED * timeStep
    local rotateQuat = Quaternion(delta, Vector3(0.0, 0.0, 1.0))
    for i, v in ipairs(boxNodes) do
        v:Rotate(rotateQuat)
    end
end

function HandleUpdate(eventType, eventData)
    -- Take the frame time step, which is stored as a float
    local timeStep = eventData:GetFloat("TimeStep")

    -- Toggle animation with space
    if input:GetKeyPress(KEY_SPACE) then
        animate = not animate
    end

    -- Move the camera, scale movement with time step
    MoveCamera(timeStep)
    
    -- Animate scene if enabled
    if animate then
        AnimateObjects(timeStep)
    end
end