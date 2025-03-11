#ifndef SANDBOX_H
#define SANDBOX_H
#include "../core/globals.h"
#include "../utils/gizmo.h"

enum inspector
{
	inspector_none,
	inspector_entity,
	inspector_material,
	inspector_settings,
	inspector_materials,
	inspector_import_model,
	inspector_import_animation
};

class sandbox : public heavy_application
{
public:
	struct
	{
		uint64_t move = 0;
		material* material = nullptr;
		entity* might = nullptr;
		entity* entity = nullptr;
		IGizmo* gizmo = nullptr;
		inspector window = inspector_none;
		string pathname;
	} selection;

	struct
	{
		std::function<void(const string&)> on_resource;
		std::function<void(entity*)> on_entity;
		gui::data_model* system = nullptr;
		gui::data_node* entities = nullptr;
		gui::data_node* materials = nullptr;
		gui::data_node* models = nullptr;
		gui::data_node* skins = nullptr;
		gui::data_node* directories = nullptr;
		gui::data_node* files = nullptr;
		gui::context* gui = nullptr;
		entity* camera = nullptr;
		entity* draggable = nullptr;
		transform::spacing space;
		string status;
		string filename;
		string target;
		vector2 cursor;
		matrix4x4 gizmo;
		float gizmo_scale = 1.0f;
		float frames = 0.0f;
		bool is_path_tracked = false;
		bool is_scene_focused = false;
		bool is_camera_active = false;
		bool is_trace_mode = false;
		bool is_draggable = false;
		bool is_drag_hovered = false;
		bool is_interactive = false;
		bool is_captured = false;
		bool is_mounted = false;
		uint32_t mesh_import_opts = 0;
		int elements_limit = 0;
	} state;

	struct
	{
		IGizmo* gizmo[3] = { nullptr };
		string current_path;
		string next_path;
		string scene_path;
	} resource;

	struct
	{
		texture_2d* sandbox = nullptr;
		texture_2d* empty = nullptr;
		texture_2d* animation = nullptr;
		texture_2d* body = nullptr;
		texture_2d* camera = nullptr;
		texture_2d* decal = nullptr;
		texture_2d* mesh = nullptr;
		texture_2d* motion = nullptr;
		texture_2d* light = nullptr;
		texture_2d* probe = nullptr;
		texture_2d* listener = nullptr;
		texture_2d* source = nullptr;
		texture_2d* emitter = nullptr;
	} icons;

	struct
	{
		surface* sandbox = nullptr;
	} favicons;

	struct
	{
		depth_stencil_state* depth_stencil = nullptr;
		rasterizer_state* none_rasterizer = nullptr;
		rasterizer_state* back_rasterizer = nullptr;
		blend_state* blend = nullptr;
		input_layout* layout = nullptr;
	} states;

public:
	explicit sandbox(heavy_application::desc* conf, const string& path = "");
	~sandbox() override;
	void key_event(key_code key, key_mod mod, int computed, int repeat, bool pressed) override;
	void window_event(window_state state, int x, int y) override;
	void initialize() override;
	void dispatch(timer* time) override;
	void publish(timer* time) override;
	void load_camera();
	void unload_camera();
	void update_scene();
	void update_grid(timer* time);
	void update_mutation(const std::string_view& name, variant_args& args);
	void update_files(const string& path, int64_t depth = 0);
	void update_system();
	void inspect_entity();
	void inspect_settings();
	void inspect_importer();
	void inspect_material();
	void set_logging(bool active);
	void set_view_model();
	void set_directory(const string& path);
	void set_selection(inspector window, void* object = nullptr);
	void set_status(const string& status);
	void set_mutation(entity* parent, const std::string_view& type);
	void set_metadata(entity* source);
	void get_path_name(string& path);
	void get_entity_cell();
	void get_entity_sync();
	void get_resource(const string& name, const std::function<void(const string&)>& callback);
	void get_entity(const string& name, const std::function<void(entity*)>& callback);
	bool get_scene_focus();
	bool get_resource_state(const string& name);
	bool get_entity_state(const string& name);
	bool get_selection_state();
	texture_2d* get_icon(entity* value);
	void* get_entity_index(entity* value);
	uint64_t get_entity_nesting(entity* value);
	string get_label(entity* value);
	string get_name(entity* value);
	string get_pascal(const string& value);
};
#endif