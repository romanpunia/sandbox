#include "sandbox.h"
#include "demo.h"
#include "../controller/resolvers.h"
#include "../controller/components.h"
#include "../controller/renderers.h"
#include "../controller/effects.h"

sandbox::sandbox(heavy_application::desc* conf, const string& path) : heavy_application(conf)
{
	os::directory::set_working(os::directory::get_module()->c_str());
	resource.next_path = path;
#ifdef _DEBUG
	console::get()->show();
#endif
}
sandbox::~sandbox()
{
	set_logging(false);
	memory::release(icons.empty);
	memory::release(icons.animation);
	memory::release(icons.body);
	memory::release(icons.camera);
	memory::release(icons.decal);
	memory::release(icons.mesh);
	memory::release(icons.motion);
	memory::release(icons.light);
	memory::release(icons.probe);
	memory::release(icons.listener);
	memory::release(icons.source);
	memory::release(icons.emitter);
	memory::release(icons.sandbox);
	memory::release(favicons.sandbox);
	delete (CGizmoTransformMove*)resource.gizmo[0];
	delete (CGizmoTransformRotate*)resource.gizmo[1];
	delete (CGizmoTransformScale*)resource.gizmo[2];
}
void sandbox::key_event(key_code key, key_mod, int, int, bool pressed)
{
	if (key == key_code::cursor_left && selection.gizmo)
	{
		if (!pressed)
		{
			activity->set_grabbing(false);
			selection.gizmo->OnMouseUp((unsigned int)state.cursor.x, (unsigned int)state.cursor.y);
		}
		else if (selection.gizmo->OnMouseDown((unsigned int)state.cursor.x, (unsigned int)state.cursor.y))
			activity->set_grabbing(true);
	}
}
void sandbox::window_event(window_state new_state, int x, int y)
{
	switch (new_state)
	{
		case window_state::resize:
			renderer->resize_buffers((unsigned int)x, (unsigned int)y);
			if (scene != nullptr)
				scene->resize_buffers();

			set_status("Buffer resize was submitted");
			break;
		case window_state::close:
			activity->message.setup(alert_type::warning, "Sandbox", "Do you really want to exit?");
			activity->message.button(alert_confirm::escape, "No", 1);
			activity->message.button(alert_confirm::defer, "Yes", 2);
			activity->message.result([this](int button)
			{
				if (button == 2)
					stop();
			});
			break;
		default:
			break;
	}
}
void sandbox::initialize()
{
	states.depth_stencil = renderer->get_depth_stencil_state("doo_soo_lt");
	states.none_rasterizer = renderer->get_rasterizer_state("so_co");
	states.back_rasterizer = renderer->get_rasterizer_state("so_cback");
	states.blend = renderer->get_blend_state("bw_wrgba_one");
	states.layout = renderer->get_input_layout("vx_shape");
	icons.sandbox = *content->load<texture_2d>("editor/img/sandbox.png");
	icons.empty = *content->load<texture_2d>("editor/img/empty.png");
	icons.animation = *content->load<texture_2d>("editor/img/animation.png");
	icons.body = *content->load<texture_2d>("editor/img/body.png");
	icons.camera = *content->load<texture_2d>("editor/img/camera.png");
	icons.decal = *content->load<texture_2d>("editor/img/decal.png");
	icons.mesh = *content->load<texture_2d>("editor/img/mesh.png");
	icons.motion = *content->load<texture_2d>("editor/img/motion.png");
	icons.light = *content->load<texture_2d>("editor/img/light.png");
	icons.probe = *content->load<texture_2d>("editor/img/probe.png");
	icons.listener = *content->load<texture_2d>("editor/img/listener.png");
	icons.source = *content->load<texture_2d>("editor/img/source.png");
	icons.emitter = *content->load<texture_2d>("editor/img/emitter.png");
	favicons.sandbox = *renderer->create_surface(icons.sandbox);
	resource.current_path = content->get_environment();
	resource.gizmo[0] = CreateMoveGizmo();
	resource.gizmo[1] = CreateRotateGizmo();
	resource.gizmo[2] = CreateScaleGizmo();
	resource.gizmo[1]->SetAxisMask(IGizmo::AXIS_X | IGizmo::AXIS_Y | IGizmo::AXIS_Z);
	state.is_interactive = true;
	state.is_path_tracked = false;
	state.is_camera_active = true;
	state.is_trace_mode = false;
	state.is_draggable = false;
	state.is_drag_hovered = false;
	state.is_captured = false;
	state.gui = fetch_ui();
	state.gui->set_mount_callback([this](gui::context*)
	{
		if (!state.is_mounted)
		{
			set_view_model();
			state.is_mounted = true;
		}
	});
	state.draggable = nullptr;
	state.gizmo_scale = 1.25f;
	state.status = "Ready";
	state.elements_limit = 1024;
	state.mesh_import_opts = (uint32_t)processors::mesh_preset::defaults;

	auto document = state.gui->load_document("editor/ui/layout.html", true);
	if (!document)
	{
		VI_ERR("could not load gui: %s", document.error().what());
		return stop();
	}

	auto* scene_processor = (processors::scene_graph_processor*)content->get_processor<scene_graph>();
	scene_processor->setup_callback = [this](scene_graph* scene)
	{
		scene->clear_listener("mutation", nullptr);
		scene->set_listener("mutation", std::bind(&sandbox::update_mutation, this, std::placeholders::_1, std::placeholders::_2));
	};

	document->show();
	resource.next_path = "./scenes/demo.xml";
	error_handling::set_flag(log_option::async, false);
	demo::set_source("");
	set_logging(true);
	update_scene();
	update_files(resource.current_path);
	set_status("Initialization done");

	activity->set_icon(favicons.sandbox);
	activity->callbacks.cursor_move = [this](int x, int y, int RX, int RY)
	{
		state.cursor = activity->get_cursor_position();
		if (selection.gizmo)
			selection.gizmo->OnMouseMove(x, y);
	};
}
void sandbox::dispatch(timer* time)
{
#ifdef _DEBUG
	if (activity->is_key_down_hit(key_code::f5) && state.gui != nullptr)
	{
		state.gui->clear_documents();
		state.gui->clear_styles();

		auto document = state.gui->load_document("editor/ui/layout.html", true);
		if (document)
			document->show();
	}
#endif
	if (activity->is_key_down_hit(key_code::f6))
		state.is_interactive = !state.is_interactive;

	if (!state.is_camera_active && scene->get_camera()->get_entity() == state.camera)
		state.is_camera_active = true;

	if (state.camera != nullptr)
	{
		bool active = (!scene->is_active() && !state.gui->is_input_focused());
		state.camera->get_component<components::fly>()->set_active(active);
		state.camera->get_component<components::free_look>()->set_active(active);
		if (active)
		{
			for (auto& item : *state.camera)
				item.second->update(time);
		}
	}

	if (state.is_interactive)
	{
		state.frames = (float)time->get_frames();
		if (!resource.next_path.empty())
			return update_scene();

		update_system();
		if (state.gui != nullptr)
			state.gui->update_events(activity);
	}

	if (!state.is_captured)
	{
		if (state.is_interactive)
		{
			if (activity->is_key_down_hit(key_code::f1))
			{
				state.is_trace_mode = !state.is_trace_mode;
				if (!state.is_trace_mode)
					set_status("Tracing mode: off");
				else
					set_status("Tracing mode: on");
			}

			if (state.camera != nullptr && scene->get_camera()->get_entity() == state.camera && state.is_camera_active)
			{
				if (selection.entity != nullptr)
				{
					if (activity->is_key_down_hit(key_code::deinit))
					{
						set_status("Entity was removed");
						scene->delete_entity(selection.entity);
						set_selection(inspector_none);
					}

					if (get_scene_focus())
					{
						if (activity->is_key_down_hit(key_code::d1))
						{
							selection.gizmo = resource.gizmo[selection.move = 0];
							selection.gizmo->SetEditMatrix(state.gizmo.row);
							selection.gizmo->SetDisplayScale(state.gizmo_scale);
							selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
							set_status("Movement gizmo selected");
						}
						else if (activity->is_key_down_hit(key_code::d2))
						{
							selection.gizmo = resource.gizmo[selection.move = 1];
							selection.gizmo->SetEditMatrix(state.gizmo.row);
							selection.gizmo->SetDisplayScale(state.gizmo_scale);
							selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
							set_status("Rotation gizmo selected");
						}
						else if (activity->is_key_down_hit(key_code::d3))
						{
							selection.gizmo = resource.gizmo[selection.move = 2];
							selection.gizmo->SetEditMatrix(state.gizmo.row);
							selection.gizmo->SetDisplayScale(state.gizmo_scale);
							selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
							set_status("Scale gizmo selected");
						}
					}
				}

				if (selection.entity != nullptr)
				{
					if (selection.gizmo != nullptr)
					{
						transform::spacing space;
						space.position = state.gizmo.position();
						space.rotation = state.gizmo.rotation_euler();
						selection.entity->get_transform()->localize(space);

						if (selection.gizmo->IsActive())
						{
							switch (selection.move)
							{
								case 1:
									selection.entity->get_transform()->set_rotation(space.rotation);
									break;
								case 2:
									selection.entity->get_transform()->set_scale(state.gizmo.scale());
									break;
								default:
									selection.entity->get_transform()->set_position(space.position);
									break;
							}
							get_entity_sync();
						}
						else
						{
							state.gizmo = selection.entity->get_transform()->get_bias();
							selection.gizmo->SetEditMatrix(state.gizmo.row);
						}
					}
					else
					{
						state.gizmo = selection.entity->get_transform()->get_bias();
						selection.gizmo = resource.gizmo[selection.move];
						selection.gizmo->SetEditMatrix(state.gizmo.row);
						selection.gizmo->SetDisplayScale(state.gizmo_scale);
						selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
					}
				}

				get_entity_cell();
				if (selection.entity != nullptr && selection.entity != state.camera)
				{
					if (activity->is_key_down_hit(key_code::deinit))
					{
						set_status("Entity was removed");
						scene->delete_entity(selection.entity);
						set_selection(inspector_none);
					}

					if (activity->is_key_down(key_mod::left_control) && get_scene_focus())
					{
						if (activity->is_key_down_hit(key_code::v) || activity->is_key_down(key_code::b))
						{
							set_status("Entity was cloned");
							entity* result = scene->clone_entity(selection.entity);
							set_selection(result ? inspector_entity : inspector_none, result);
						}

						if (activity->is_key_down_hit(key_code::x))
						{
							set_status("Entity was removed");
							scene->delete_entity(selection.entity);
							set_selection(inspector_none);
						}
					}
				}

				if (activity->is_key_down(key_mod::left_control))
				{
					if (activity->is_key_down(key_code::n))
					{
						entity* last = scene->get_last_entity();
						if (last != nullptr && last != state.camera)
						{
							if (selection.entity == last)
								set_selection(inspector_none);

							set_status("Last entity was removed");
							scene->delete_entity(last);
						}
					}

					if (activity->is_key_down(key_code::x))
					{
						if (selection.entity != nullptr)
						{
							set_status("Entity was removed");
							scene->delete_entity(selection.entity);
							set_selection(inspector_none);
						}
					}
				}
			}
		}
	}
	else
		state.is_captured = false;

	scene->dispatch(time);
}
void sandbox::publish(timer* time)
{
	renderer->clear(0, 0, 0);
	renderer->clear_depth();

	scene->publish(time);
	if (state.is_interactive && state.camera == scene->get_camera()->get_entity())
	{
		scene->set_mrt(target_type::main, false);
		update_grid(time);
	}

	scene->submit();
	if (state.is_interactive && state.gui != nullptr)
		state.gui->render_lists(renderer->get_render_target()->get_target());

	renderer->submit();
}
void sandbox::load_camera()
{
	state.is_camera_active = true;
	state.camera = scene->add_entity();
	state.camera->add_component<components::camera>();
	state.camera->add_component<components::free_look>();
	state.camera->get_transform()->set_spacing(positioning::global, state.space);

	auto* fly = state.camera->add_component<components::fly>();
	fly->moving.slower *= 0.25f;
	fly->moving.normal *= 0.35f;
	scene->set_camera(state.camera);

	auto* fRenderer = scene->get_renderer();
	fRenderer->add_renderer<renderers::model>();
	fRenderer->add_renderer<renderers::skin>();
	fRenderer->add_renderer<renderers::soft_body>();
	fRenderer->add_renderer<renderers::emitter>();
	fRenderer->add_renderer<renderers::decal>();
	fRenderer->add_renderer<renderers::lighting>();
	fRenderer->add_renderer<renderers::transparency>();
}
void sandbox::unload_camera()
{
	state.is_camera_active = false;
	if (!state.camera)
		return;

	state.space = state.camera->get_transform()->get_spacing(positioning::global);
	if (scene != nullptr)
		scene->delete_entity(state.camera);
	state.camera = nullptr;
}
void sandbox::update_scene()
{
	set_selection(inspector_none);
	unload_camera();

	state.entities->clear();
	state.materials->clear();
	memory::release(scene);

	vm->clear_cache();
	if (!resource.next_path.empty())
	{
		variant_args args;
		args["active"] = var::boolean(false);
		args["mutations"] = var::boolean(true);
		scene = content->load<scene_graph>(resource.next_path, args).or_else(nullptr);
	}

	if (scene == nullptr)
	{
		scene_graph::desc i = scene_graph::desc::get(this);
		i.simulator.enable_soft_body = true;
		i.mutations = true;

		scene = new scene_graph(i);
		scene->set_active(false);

		set_status("Created new scene from scratch");
	}
	else
		set_status("Scene was loaded");

	resource.scene_path = resource.next_path;
	resource.next_path.clear();
	load_camera();
}
void sandbox::update_grid(timer* time)
{
	matrix4x4 origin[4] =
	{
		matrix4x4::identity(),
		matrix4x4::create_translation({ 0, 0, -2 }),
		matrix4x4::create({ 0, 0, 0 }, 1, { 0, math<float>::deg2rad() * (90), 0 }),
		matrix4x4::create({ -2, 0, 0 }, 1, { 0, math<float>::deg2rad() * (90), 0 })
	};

	renderer->set_depth_stencil_state(states.depth_stencil);
	renderer->set_blend_state(states.blend);
	renderer->set_rasterizer_state(states.back_rasterizer);
	renderer->set_input_layout(states.layout);
	renderer->set_shader(constants->get_basic_effect(), VI_VS | VI_PS);
	renderer->set_vertex_buffer(cache.primitives->get_quad());

	for (uint32_t i = 0; i < scene->get_entities_count(); i++)
	{
		entity* value = scene->get_entity(i);
		if (state.camera == value)
			continue;

		auto& from = value->get_transform()->get_position();
		auto& to = state.camera->get_transform()->get_position();
		float direction = vector2(from.x, from.z).look_at(vector2(to.x, to.z));
		constants->render.texcoord = (value == selection.entity ? 0.5f : 0.05f);
		constants->render.transform = matrix4x4::create(value->get_transform()->get_position(), 0.5f, vector3(0, direction)) * state.camera->get_component<components::camera>()->get_view_projection();
		constants->update_constant_buffer(render_buffer_type::render);
		renderer->set_texture_2d(get_icon(value), 1, VI_PS);
		renderer->draw(6, 0);
	}

	if (selection.gizmo != nullptr)
	{
		renderer->set_rasterizer_state(states.none_rasterizer);
		vector2 size = activity->get_size();
		selection.gizmo->SetScreenDimension((int)size.x, (int)size.y);
		selection.gizmo->SetCameraMatrix(((components::camera*)scene->get_camera())->get_view().row, ((components::camera*)scene->get_camera())->get_projection().row);
		selection.gizmo->Draw();
		renderer->set_rasterizer_state(states.back_rasterizer);
	}

	for (uint32_t i = 0; i < scene->get_entities_count(); i++)
	{
		entity* value = scene->get_entity(i);
		if (state.camera == value)
			continue;

		if (!state.is_trace_mode)
		{
			if (value != selection.entity && value != selection.might)
				continue;
		}

		if (state.is_path_tracked)
		{
			auto* key_animator = value->get_component<components::key_animator>();
			if (key_animator != nullptr && key_animator->is_exists(key_animator->state.clip))
			{
				vector<animator_key>* keys = key_animator->get_clip(key_animator->state.clip);
				if (keys != nullptr)
				{
					matrix4x4 offset = (value->get_transform()->get_root() ? value->get_transform()->get_root()->get_bias() : matrix4x4::identity());
					for (size_t j = 0; j < keys->size(); j++)
					{
						auto& pos = keys->at(j).position;
						renderer->im_begin();
						renderer->im_topology(primitive_topology::line_strip);
						renderer->im_transform(offset * ((components::camera*)scene->get_camera())->get_view_projection());
						renderer->im_emit();
						renderer->im_position(pos.x, pos.y, -pos.z);
						renderer->im_emit();

						if (key_animator->state.frame == j)
							renderer->im_color(1, 0, 1, 1);

						if (j + 1 >= keys->size())
						{
							auto& xPos = keys->at(0).position;
							renderer->im_position(xPos.x, xPos.y, -xPos.z);
						}
						else
						{
							auto& xPos = keys->at(j + 1).position;
							renderer->im_position(xPos.x, xPos.y, -xPos.z);
						}

						renderer->im_end();
					}
				}
			}
		}

		matrix4x4 transform = value->get_box();
		if (value->get_component<components::camera>() != nullptr)
		{
			auto& global = value->get_transform()->get_spacing();
			transform = matrix4x4::create(global.position, global.scale, global.rotation.inv_y());
		}
		for (int j = 0; j < 4; j++)
		{
			renderer->im_begin();
			renderer->im_topology(primitive_topology::line_strip);
			renderer->im_transform(origin[j] * transform * ((components::camera*)scene->get_camera())->get_view_projection());
			renderer->im_emit();
			if (value == selection.might)
				renderer->im_color(1, 1, 0.75f, 0.15f);
			else if (value != selection.entity)
				renderer->im_color(1, 1, 1, 0.1f);
			else
				renderer->im_color(1, 1, 1, 0.5f);
			renderer->im_position(1, 1, 1);

			renderer->im_emit();
			if (value == selection.might)
				renderer->im_color(1, 1, 0.75f, 0.15f);
			else if (value != selection.entity)
				renderer->im_color(1, 1, 1, 0.1f);
			else
				renderer->im_color(1, 1, 1, 0.5f);
			renderer->im_position(-1, 1, 1);

			renderer->im_emit();
			if (value == selection.might)
				renderer->im_color(1, 1, 0.75f, 0.15f);
			else if (value != selection.entity)
				renderer->im_color(1, 1, 1, 0.1f);
			else
				renderer->im_color(1, 1, 1, 0.5f);
			renderer->im_position(-1, -1, 1);

			renderer->im_emit();
			if (value == selection.might)
				renderer->im_color(1, 1, 0.75f, 0.15f);
			else if (value != selection.entity)
				renderer->im_color(1, 1, 1, 0.1f);
			else
				renderer->im_color(1, 1, 1, 0.5f);
			renderer->im_position(1, -1, 1);

			renderer->im_emit();
			if (value == selection.might)
				renderer->im_color(1, 1, 0.75f, 0.15f);
			else if (value != selection.entity)
				renderer->im_color(1, 1, 1, 0.1f);
			else
				renderer->im_color(1, 1, 1, 0.5f);
			renderer->im_position(1, 1, 1);
			renderer->im_end();
		}
	}

	if (selection.material != nullptr && selection.window == inspector_material)
	{
		matrix4x4 view_projection = ((components::camera*)scene->get_camera())->get_view_projection();
		for (uint32_t i = 0; i < scene->get_entities_count(); i++)
		{
			entity* entity = scene->get_entity(i);
			material* source = nullptr;

			if (entity->get_component<components::model>())
			{
				auto* it = entity->get_component<components::model>();
				for (auto& face : it->get_materials())
				{
					if (face.second == selection.material)
					{
						source = face.second;
						break;
					}
				}
			}
			else if (entity->get_component<components::skin>())
			{
				auto* it = entity->get_component<components::skin>();
				for (auto& face : it->get_materials())
				{
					if (face.second == selection.material)
					{
						source = face.second;
						break;
					}
				}
			}
			else if (entity->get_component<components::emitter>())
				source = entity->get_component<components::emitter>()->get_material();
			else if (entity->get_component<components::soft_body>())
				source = entity->get_component<components::soft_body>()->get_material();
			else if (entity->get_component<components::decal>())
				source = entity->get_component<components::decal>()->get_material();

			if (source != selection.material)
				continue;

			for (const auto& j : origin)
			{
				renderer->im_begin();
				renderer->im_topology(primitive_topology::line_strip);
				renderer->im_transform(j * scene->get_entity(i)->get_transform()->get_bias() * view_projection);
				renderer->im_emit();
				renderer->im_color(0.5f, 0.5f, 1.0f, 0.75f);
				renderer->im_position(1.0f, 1.0f, 1.0f);
				renderer->im_emit();
				renderer->im_color(0.5f, 0.5f, 1.0f, 0.75f);
				renderer->im_position(-1.0f, 1.0f, 1.0f);
				renderer->im_emit();
				renderer->im_color(0.5f, 0.5f, 1.0f, 0.75f);
				renderer->im_position(-1.0f, -1.0f, 1.0f);
				renderer->im_emit();
				renderer->im_color(0.5f, 0.5f, 1.0f, 0.75f);
				renderer->im_position(1.0f, -1.0f, 1.0f);
				renderer->im_emit();
				renderer->im_color(0.5f, 0.5f, 1.0f, 0.75f);
				renderer->im_position(1.0f, 1.0f, 1.0f);
				renderer->im_end();
			}
		}
	}
}
void sandbox::update_mutation(const std::string_view& name, variant_args& args)
{
	if (args.find("entity") != args.end())
	{
		entity* base = (entity*)args["entity"].get_pointer();
		if (!base || base == state.camera || !state.entities)
			return;

		for (size_t i = 0; i < state.entities->size(); i++)
		{
			gui::data_node& node = state.entities->at(i);
			if (gui::ielement::to_pointer(node.at(1).get_string()) == (void*)base)
			{
				state.entities->remove(i);
				break;
			}
		}

		if (args["type"].is_string("pop") || (!args["type"].is_string("set") && !args["type"].is_string("push")))
			return;

		auto top = std::make_pair(get_entity_index(base), (size_t)get_entity_nesting(base));

		variant_list item;
		item.emplace_back(std::move(var::integer(top.second)));
		item.emplace_back(std::move(var::string(gui::ielement::from_pointer((void*)base))));
		item.emplace_back(std::move(var::string(get_name(base))));
		state.entities->add(item, &top);
	}
	else if (args.find("parent") != args.end())
	{
		entity* base = (entity*)args["child"].get_pointer();
		if (!base || base == state.camera || !state.entities)
			return;

		auto top = std::make_pair(get_entity_index(base), (size_t)get_entity_nesting(base));

		variant_list item;
		item.emplace_back(std::move(var::integer(get_entity_nesting(base))));
		item.emplace_back(std::move(var::string(gui::ielement::from_pointer((void*)base))));
		item.emplace_back(std::move(var::string(get_name(base))));

		for (size_t i = 0; i < state.entities->size(); i++)
		{
			gui::data_node& node = state.entities->at(i);
			if (gui::ielement::to_pointer(node.at(1).get_string()) == (void*)base)
			{
				node.replace(item, &top);
				state.entities->sort_tree();
				set_mutation(base, args["type"].get_string());
				return;
			}
		}

		state.entities->add(item, &top);
	}
	else if (args.find("component") != args.end())
	{
		component* child = (component*)args["component"].get_pointer();
		if (!child || child->get_entity() == state.camera || !state.entities)
			return;

		entity* base = child->get_entity();
		for (size_t i = 0; i < state.entities->size(); i++)
		{
			gui::data_node& node = state.entities->at(i);
			if (gui::ielement::to_pointer(node.at(1).get_string()) == (void*)base)
			{
				state.entities->remove(i);
				break;
			}
		}

		auto top = std::make_pair(get_entity_index(base), (size_t)get_entity_nesting(base));

		variant_list item;
		item.emplace_back(std::move(var::integer(top.second)));
		item.emplace_back(std::move(var::string(gui::ielement::from_pointer((void*)base))));
		item.emplace_back(std::move(var::string(get_name(base))));
		state.entities->add(item, &top);
	}
	else if (args.find("material") != args.end())
	{
		material* base = (material*)args["material"].get_pointer();
		if (!base || !state.materials || (scene && base == scene->get_invalid_material()))
			return;

		for (size_t i = 0; i < state.materials->size(); i++)
		{
			gui::data_node& node = state.materials->at(i);
			if (gui::ielement::to_pointer(node.at(0).get_string()) == (void*)base)
			{
				state.materials->remove(i);
				break;
			}
		}

		if (!args["type"].is_string("push") && !args["type"].is_string("set"))
			return;

		variant_list item;
		item.emplace_back(std::move(var::string(gui::ielement::from_pointer((void*)base))));
		item.emplace_back(std::move(var::string(base->get_name().empty() ? "Unnamed material" : base->get_name())));
		state.materials->add(item);
	}
}
void sandbox::update_files(const string& path, int64_t depth)
{
	if (depth <= 0)
		state.directories->clear();

	vector<std::pair<string, file_entry>> files;
	if (!os::directory::scan(path, files) || files.empty())
		return;

	string subpath = path;
	for (auto& file : files)
	{
		if (depth <= 0 && (file.first == "editor" || file.first == "shaders"))
			continue;
		else if (!file.second.is_directory)
			continue;

		auto top = std::make_pair((void*)(uintptr_t)state.directories->size(), (size_t)depth);
		string pathname = subpath + file.first;
		string filename = pathname;
		get_path_name(filename);

		variant_list item;
		item.emplace_back(std::move(var::string(filename)));
		item.emplace_back(std::move(var::string(pathname)));
		item.emplace_back(std::move(var::integer(depth)));
		state.directories->add(item);
		update_files(filename, depth + 1);
	}
}
void sandbox::update_system()
{
	state.system->set_boolean("scene_active", scene->is_active());
	state.system->set_boolean("sl_resource", !state.filename.empty());
	state.system->set_string("sl_material", gui::ielement::from_pointer((void*)selection.material));
	state.system->set_string("sl_entity", gui::ielement::from_pointer((void*)selection.entity));
	state.system->set_string("sl_status", state.status);

	switch (selection.window)
	{
		case inspector_entity:
			state.system->set_string("sl_window", "entity");
			inspect_entity();
			break;
		case inspector_material:
			state.system->set_string("sl_window", "material");
			inspect_material();
			break;
		case inspector_settings:
			state.system->set_string("sl_window", "settings");
			inspect_settings();
			break;
		case inspector_materials:
			state.system->set_string("sl_window", "materials");
			break;
		case inspector_import_model:
			state.system->set_string("sl_window", "import-model");
			inspect_importer();
			break;
		case inspector_import_animation:
			state.system->set_string("sl_window", "import-animation");
			inspect_importer();
			break;
		case inspector_none:
		default:
			state.system->set_string("sl_window", "none");
			break;
	}
}
void sandbox::inspect_entity()
{
	static entity* last_base = nullptr;
	if (!selection.entity)
		return;

	entity* base = selection.entity;
	bool changed = (last_base != base);
	if (changed)
		last_base = base;

	string name = base->get_name();
	if (state.gui->get_element_by_id("ent_name").cast_form_string(&name))
		base->set_name(name);

	auto& space = base->get_transform()->get_spacing();
	if (state.gui->get_element_by_id("ent_pos_x").cast_form_float(&space.position.x) ||
		state.gui->get_element_by_id("ent_pos_y").cast_form_float(&space.position.y) ||
		state.gui->get_element_by_id("ent_pos_z").cast_form_float(&space.position.z) ||
		state.gui->get_element_by_id("ent_rot_x").cast_form_float(&space.rotation.x, mathf::rad2deg()) ||
		state.gui->get_element_by_id("ent_rot_y").cast_form_float(&space.rotation.y, mathf::rad2deg()) ||
		state.gui->get_element_by_id("ent_rot_z").cast_form_float(&space.rotation.z, mathf::rad2deg()) ||
		state.gui->get_element_by_id("ent_scale_x").cast_form_float(&space.scale.x) ||
		state.gui->get_element_by_id("ent_scale_y").cast_form_float(&space.scale.y) ||
		state.gui->get_element_by_id("ent_scale_z").cast_form_float(&space.scale.z))
	{
		base->get_transform()->make_dirty();
		get_entity_sync();
	}

	bool scaling = base->get_transform()->has_scaling();
	if (state.gui->get_element_by_id("ent_const_scale").cast_form_boolean(&scaling))
		base->get_transform()->set_scaling(scaling);

	if (state.system->set_boolean("sl_cmp_model", base->get_component<components::model>() != nullptr)->get_boolean())
		component_model(state.gui, base->get_component<components::model>(), changed);

	if (state.system->set_boolean("sl_cmp_skin", base->get_component<components::skin>() != nullptr)->get_boolean())
		component_skin(state.gui, base->get_component<components::skin>(), changed);

	if (state.system->set_boolean("sl_cmp_emitter", base->get_component<components::emitter>() != nullptr)->get_boolean())
		component_emitter(state.gui, base->get_component<components::emitter>(), changed);

	if (state.system->set_boolean("sl_cmp_soft_body", base->get_component<components::soft_body>() != nullptr)->get_boolean())
		component_soft_body(state.gui, base->get_component<components::soft_body>(), changed);

	if (state.system->set_boolean("sl_cmp_decal", base->get_component<components::decal>() != nullptr)->get_boolean())
		component_decal(state.gui, base->get_component<components::decal>(), changed);

	if (state.system->set_boolean("sl_cmp_skin_animator", base->get_component<components::skin_animator>() != nullptr)->get_boolean())
		component_skin_animator(state.gui, base->get_component<components::skin_animator>(), changed);

	if (state.system->set_boolean("sl_cmp_key_animator", base->get_component<components::key_animator>() != nullptr)->get_boolean())
		component_key_animator(state.gui, base->get_component<components::key_animator>(), changed);

	if (state.system->set_boolean("sl_cmp_emitter_animator", base->get_component<components::emitter_animator>() != nullptr)->get_boolean())
		component_emitter_animator(state.gui, base->get_component<components::emitter_animator>(), changed);

	if (state.system->set_boolean("sl_cmp_rigid_body", base->get_component<components::rigid_body>() != nullptr)->get_boolean())
		component_rigid_body(state.gui, base->get_component<components::rigid_body>(), changed);

	if (state.system->set_boolean("sl_cmp_acceleration", base->get_component<components::acceleration>() != nullptr)->get_boolean())
		component_acceleration(state.gui, base->get_component<components::acceleration>(), changed);

	if (state.system->set_boolean("sl_cmp_slider_constraint", base->get_component<components::slider_constraint>() != nullptr)->get_boolean())
		component_slider_constraint(state.gui, base->get_component<components::slider_constraint>(), changed);

	if (state.system->set_boolean("sl_cmp_free_look", base->get_component<components::free_look>() != nullptr)->get_boolean())
		component_free_look(state.gui, base->get_component<components::free_look>(), changed);

	if (state.system->set_boolean("sl_cmp_fly", base->get_component<components::fly>() != nullptr)->get_boolean())
		component_fly(state.gui, base->get_component<components::fly>(), changed);

	if (state.system->set_boolean("sl_cmp_audio_source", base->get_component<components::audio_source>() != nullptr)->get_boolean())
	{
		auto* source = base->get_component<components::audio_source>();
		state.system->set_boolean("sl_cmp_audio_source_compressor", source->get_source()->get_effect<effects::compressor>() != nullptr);

		if (state.system->set_boolean("sl_cmp_audio_source_reverb", source->get_source()->get_effect<effects::reverb>() != nullptr)->get_boolean())
			effect_reverb(state.gui, source->get_source()->get_effect<effects::reverb>());

		if (state.system->set_boolean("sl_cmp_audio_source_chorus", source->get_source()->get_effect<effects::chorus>() != nullptr)->get_boolean())
			effect_chorus(state.gui, source->get_source()->get_effect<effects::chorus>());

		if (state.system->set_boolean("sl_cmp_audio_source_distortion", source->get_source()->get_effect<effects::distortion>() != nullptr)->get_boolean())
			effect_distortion(state.gui, source->get_source()->get_effect<effects::distortion>());

		if (state.system->set_boolean("sl_cmp_audio_source_echo", source->get_source()->get_effect<effects::echo>() != nullptr)->get_boolean())
			effect_echo(state.gui, source->get_source()->get_effect<effects::echo>());

		if (state.system->set_boolean("sl_cmp_audio_source_flanger", source->get_source()->get_effect<effects::flanger>() != nullptr)->get_boolean())
			effect_flanger(state.gui, source->get_source()->get_effect<effects::flanger>());

		if (state.system->set_boolean("sl_cmp_audio_source_frequency_shifter", source->get_source()->get_effect<effects::frequency_shifter>() != nullptr)->get_boolean())
			effect_frequency_shifter(state.gui, source->get_source()->get_effect<effects::frequency_shifter>());

		if (state.system->set_boolean("sl_cmp_audio_source_vocal_morpher", source->get_source()->get_effect<effects::vocal_morpher>() != nullptr)->get_boolean())
			effect_vocal_morpher(state.gui, source->get_source()->get_effect<effects::vocal_morpher>());

		if (state.system->set_boolean("sl_cmp_audio_source_pitch_shifter", source->get_source()->get_effect<effects::pitch_shifter>() != nullptr)->get_boolean())
			effect_pitch_shifter(state.gui, source->get_source()->get_effect<effects::pitch_shifter>());

		if (state.system->set_boolean("sl_cmp_audio_source_ring_modulator", source->get_source()->get_effect<effects::ring_modulator>() != nullptr)->get_boolean())
			effect_ring_modulator(state.gui, source->get_source()->get_effect<effects::ring_modulator>());

		if (state.system->set_boolean("sl_cmp_audio_source_autowah", source->get_source()->get_effect<effects::autowah>() != nullptr)->get_boolean())
			effect_autowah(state.gui, source->get_source()->get_effect<effects::autowah>());

		if (state.system->set_boolean("sl_cmp_audio_source_equalizer", source->get_source()->get_effect<effects::equalizer>() != nullptr)->get_boolean())
			effect_equalizer(state.gui, source->get_source()->get_effect<effects::equalizer>());

		component_audio_source(state.gui, source, changed);
	}

	if (state.system->set_boolean("sl_cmp_audio_listener", base->get_component<components::audio_listener>() != nullptr)->get_boolean())
		component_audio_listener(state.gui, base->get_component<components::audio_listener>(), changed);

	if (state.system->set_boolean("sl_cmp_point_light", base->get_component<components::point_light>() != nullptr)->get_boolean())
		component_point_light(state.gui, base->get_component<components::point_light>(), changed);

	if (state.system->set_boolean("sl_cmp_spot_light", base->get_component<components::spot_light>() != nullptr)->get_boolean())
		component_spot_light(state.gui, base->get_component<components::spot_light>(), changed);

	if (state.system->set_boolean("sl_cmp_line_light", base->get_component<components::line_light>() != nullptr)->get_boolean())
		component_line_light(state.gui, base->get_component<components::line_light>(), changed);

	if (state.system->set_boolean("sl_cmp_surface_light", base->get_component<components::surface_light>() != nullptr)->get_boolean())
		component_surface_light(state.gui, base->get_component<components::surface_light>(), changed);

	if (state.system->set_boolean("sl_cmp_illuminator", base->get_component<components::illuminator>() != nullptr)->get_boolean())
		component_illuminator(state.gui, base->get_component<components::illuminator>(), changed);

	if (state.system->set_boolean("sl_cmp_camera", base->get_component<components::camera>() != nullptr)->get_boolean())
	{
		auto* camera = base->get_component<components::camera>();
		auto* fRenderer = camera->get_renderer();
		state.system->set_integer("sl_cmp_camera_model", fRenderer->get_offset<renderers::model>());
		state.system->set_integer("sl_cmp_camera_skin", fRenderer->get_offset<renderers::skin>());
		state.system->set_integer("sl_cmp_camera_soft_body", fRenderer->get_offset<renderers::soft_body>());
		state.system->set_integer("sl_cmp_camera_decal", fRenderer->get_offset<renderers::decal>());
		state.system->set_integer("sl_cmp_camera_emitter", fRenderer->get_offset<renderers::emitter>());
		state.system->set_integer("sl_cmp_camera_gui", fRenderer->get_offset<renderers::user_interface>());
		state.system->set_integer("sl_cmp_camera_transparency", fRenderer->get_offset<renderers::transparency>());

		if (state.system->set_integer("sl_cmp_camera_lighting", fRenderer->get_offset<renderers::lighting>())->get_integer() >= 0)
			renderer_lighting(state.gui, fRenderer->get_renderer<renderers::lighting>());

		if (state.system->set_integer("sl_cmp_camera_sslr", fRenderer->get_offset<renderers::local_reflections>())->get_integer() >= 0)
			renderer_sslr(state.gui, fRenderer->get_renderer<renderers::local_reflections>());

		if (state.system->set_integer("sl_cmp_camera_ssli", fRenderer->get_offset<renderers::local_illumination>())->get_integer() >= 0)
			renderer_ssli(state.gui, fRenderer->get_renderer<renderers::local_illumination>());

		if (state.system->set_integer("sl_cmp_camera_ssla", fRenderer->get_offset<renderers::local_ambient>())->get_integer() >= 0)
			renderer_ssla(state.gui, fRenderer->get_renderer<renderers::local_ambient>());

		if (state.system->set_integer("sl_cmp_camera_motionblur", fRenderer->get_offset<renderers::motion_blur>())->get_integer() >= 0)
			renderer_motion_blur(state.gui, fRenderer->get_renderer<renderers::motion_blur>());

		if (state.system->set_integer("sl_cmp_camera_bloom", fRenderer->get_offset<renderers::bloom>())->get_integer() >= 0)
			renderer_bloom(state.gui, fRenderer->get_renderer<renderers::bloom>());

		if (state.system->set_integer("sl_cmp_camera_dof", fRenderer->get_offset<renderers::depth_of_field>())->get_integer() >= 0)
			renderer_dof(state.gui, fRenderer->get_renderer<renderers::depth_of_field>());

		if (state.system->set_integer("sl_cmp_camera_tone", fRenderer->get_offset<renderers::tone>())->get_integer() >= 0)
			renderer_tone(state.gui, fRenderer->get_renderer<renderers::tone>());

		if (state.system->set_integer("sl_cmp_camera_glitch", fRenderer->get_offset<renderers::glitch>())->get_integer() >= 0)
			renderer_glitch(state.gui, fRenderer->get_renderer<renderers::glitch>());

		component_camera(state.gui, camera, changed);
	}

	if (state.system->set_boolean("sl_cmp_scriptable", base->get_component<components::scriptable>() != nullptr)->get_boolean())
		component_scriptable(state.gui, base->get_component<components::scriptable>(), changed);
}
void sandbox::inspect_settings()
{
	scene_graph::desc& conf = scene->get_conf();
	state.gui->get_element_by_id("sc_start_grav_x").cast_form_float(&conf.simulator.gravity.x);
	state.gui->get_element_by_id("sc_start_grav_y").cast_form_float(&conf.simulator.gravity.y);
	state.gui->get_element_by_id("sc_start_grav_z").cast_form_float(&conf.simulator.gravity.z);
	state.gui->get_element_by_id("sc_wet_norm_x").cast_form_float(&conf.simulator.water_normal.x);
	state.gui->get_element_by_id("sc_wet_norm_y").cast_form_float(&conf.simulator.water_normal.y);
	state.gui->get_element_by_id("sc_wet_norm_z").cast_form_float(&conf.simulator.water_normal.z);
	state.gui->get_element_by_id("sc_air_dens").cast_form_float(&conf.simulator.air_density);
	state.gui->get_element_by_id("sc_wet_dens").cast_form_float(&conf.simulator.water_density);
	state.gui->get_element_by_id("sc_wet_off").cast_form_float(&conf.simulator.water_offset);
	state.gui->get_element_by_id("sc_max_disp").cast_form_float(&conf.simulator.max_displacement);
	state.gui->get_element_by_id("sc_softs").cast_form_boolean(&conf.simulator.enable_soft_body);
	state.gui->get_element_by_id("sc_gp_qual").cast_form_float(&conf.render_quality, 100.0f);
	state.gui->get_element_by_id("sc_gp_hdr").cast_form_boolean(&conf.enable_hdr);
	state.gui->get_element_by_id("sc_start_mats").cast_form_size(&conf.start_entities);
	state.gui->get_element_by_id("sc_start_ents").cast_form_size(&conf.start_components);
	state.gui->get_element_by_id("sc_start_comps").cast_form_size(&conf.start_materials);
	state.gui->get_element_by_id("sc_grow_marg").cast_form_size(&conf.grow_margin);
	state.gui->get_element_by_id("sc_grow_rate").cast_form_double(&conf.grow_rate);
	state.gui->get_element_by_id("sc_sp_plr").cast_form_size(&conf.points_size);
	state.gui->get_element_by_id("sc_sp_pll").cast_form_size(&conf.points_max);
	state.gui->get_element_by_id("sc_sp_slr").cast_form_size(&conf.spots_size);
	state.gui->get_element_by_id("sc_sp_sll").cast_form_size(&conf.spots_max);
	state.gui->get_element_by_id("sc_sp_llr").cast_form_size(&conf.lines_size);
	state.gui->get_element_by_id("sc_sp_lll").cast_form_size(&conf.lines_max);

	vector3 gravity = scene->get_simulator()->get_gravity();
	if (state.gui->get_element_by_id("sc_sim_grav_x").cast_form_float(&gravity.x) ||
		state.gui->get_element_by_id("sc_sim_grav_y").cast_form_float(&gravity.y) ||
		state.gui->get_element_by_id("sc_sim_grav_z").cast_form_float(&gravity.z))
		scene->get_simulator()->set_gravity(gravity);

	state.gui->get_element_by_id("sc_sim_time").cast_form_float(&scene->get_simulator()->speedup);
	state.gui->get_element_by_id("sc_sim").cast_form_boolean(&scene->get_simulator()->active);
}
void sandbox::inspect_importer()
{
	state.gui->get_element_by_id("im_jiv").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::join_identical_vertices);
	state.gui->get_element_by_id("im_tri").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::triangulate);
	state.gui->get_element_by_id("im_cts").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::calc_tangent_space);
	state.gui->get_element_by_id("im_rc").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::remove_component);
	state.gui->get_element_by_id("im_mn").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::gen_normals);
	state.gui->get_element_by_id("im_msn").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::gen_smooth_normals);
	state.gui->get_element_by_id("im_slm").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::split_large_meshes);
	state.gui->get_element_by_id("im_ptv").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::pre_transform_vertices);
	state.gui->get_element_by_id("im_vds").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::validate_data_structure);
	state.gui->get_element_by_id("im_mlh").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::make_left_handed);
	state.gui->get_element_by_id("im_icl").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::improve_cache_locality);
	state.gui->get_element_by_id("im_rrm").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::remove_redundant_materials);
	state.gui->get_element_by_id("im_fin").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::fix_infacing_normals);
	state.gui->get_element_by_id("im_sbt").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::sort_by_ptype);
	state.gui->get_element_by_id("im_rd").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::remove_degenerates);
	state.gui->get_element_by_id("im_ri").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::remove_instances);
	state.gui->get_element_by_id("im_rid").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::remove_invalid_data);
	state.gui->get_element_by_id("im_muv").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::gen_uv_coords);
	state.gui->get_element_by_id("im_tuv").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::transform_uv_coords);
	state.gui->get_element_by_id("im_om").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::optimize_meshes);
	state.gui->get_element_by_id("im_og").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::optimize_graph);
	state.gui->get_element_by_id("im_fuv").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::flip_uvs);
	state.gui->get_element_by_id("im_fwo").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::flip_winding_order);
	state.gui->get_element_by_id("im_sbb").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::split_by_bone_count);
	state.gui->get_element_by_id("im_dbn").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::debone);
	state.gui->get_element_by_id("im_gs").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::global_scale);
	state.gui->get_element_by_id("im_fmn").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::force_gen_normals);
	state.gui->get_element_by_id("im_et").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::embed_textures);
	state.gui->get_element_by_id("im_dn").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::drop_normals);
	state.gui->get_element_by_id("im_cbb").cast_form_flag32(&state.mesh_import_opts, (uint32_t)processors::mesh_opt::gen_bounding_boxes);
}
void sandbox::inspect_material()
{
	material* base = selection.material;
	if (!base)
		return;

	resolve_texture_2d(state.gui, "mat_diffuse_source", base->get_diffuse_map() != nullptr, [base](texture_2d* init)
	{
		base->set_diffuse_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_normal_source", base->get_normal_map() != nullptr, [base](texture_2d* init)
	{
		base->set_normal_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_metallic_source", base->get_metallic_map() != nullptr, [base](texture_2d* init)
	{
		base->set_metallic_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_roughness_source", base->get_roughness_map() != nullptr, [base](texture_2d* init)
	{
		base->set_roughness_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_height_source", base->get_height_map() != nullptr, [base](texture_2d* init)
	{
		base->set_height_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_occlusion_source", base->get_occlusion_map() != nullptr, [base](texture_2d* init)
	{
		base->set_occlusion_map(init);
		memory::release(init);
	}, false);
	resolve_texture_2d(state.gui, "mat_emission_source", base->get_emission_map() != nullptr, [base](texture_2d* init)
	{
		base->set_emission_map(init);
		memory::release(init);
	}, false);

	resolve_color3(state.gui, "mat_diffuse", &base->surface.diffuse);
	resolve_color3(state.gui, "mat_scat", &base->surface.scattering);
	if (state.gui->get_element_by_id("mat_cemn").cast_form_color(&base->surface.emission, false))
		state.gui->get_element_by_id("mat_cemn_color").set_property("background-color", stringify::text("rgb(%u, %u, %u)", (unsigned int)(base->surface.emission.x * 255.0f), (unsigned int)(base->surface.emission.y * 255.0f), (unsigned int)(base->surface.emission.z * 255.0f)));

	if (state.gui->get_element_by_id("mat_cmet").cast_form_color(&base->surface.metallic, false))
		state.gui->get_element_by_id("mat_cmet_color").set_property("background-color", stringify::text("rgb(%u, %u, %u)", (unsigned int)(base->surface.metallic.x * 255.0f), (unsigned int)(base->surface.metallic.y * 255.0f), (unsigned int)(base->surface.metallic.z * 255.0f)));

	if (state.gui->get_element_by_id("mat_pens").cast_form_color(&base->surface.penetration, false))
		state.gui->get_element_by_id("mat_pens_color").set_property("background-color", stringify::text("rgb(%u, %u, %u)", (unsigned int)(base->surface.penetration.x * 255.0f), (unsigned int)(base->surface.penetration.y * 255.0f), (unsigned int)(base->surface.penetration.z * 255.0f)));

	string name = base->get_name();
	if (state.gui->get_element_by_id("mat_name").cast_form_string(&name))
		base->set_name(name);

	state.gui->get_element_by_id("mat_pensv").cast_form_float(&base->surface.penetration.w);
	state.gui->get_element_by_id("mat_memn").cast_form_float(&base->surface.emission.w);
	state.gui->get_element_by_id("mat_mmet").cast_form_float(&base->surface.metallic.w);
	state.gui->get_element_by_id("mat_rs").cast_form_float(&base->surface.roughness.x);
	state.gui->get_element_by_id("mat_mrs").cast_form_float(&base->surface.roughness.y);
	state.gui->get_element_by_id("mat_occ").cast_form_float(&base->surface.occlusion.x);
	state.gui->get_element_by_id("mat_mocc").cast_form_float(&base->surface.occlusion.y);
	state.gui->get_element_by_id("mat_fres").cast_form_float(&base->surface.fresnel);
	state.gui->get_element_by_id("mat_refr").cast_form_float(&base->surface.refraction);
	state.gui->get_element_by_id("mat_alpha").cast_form_float(&base->surface.transparency);
	state.gui->get_element_by_id("mat_env").cast_form_float(&base->surface.environment);
	state.gui->get_element_by_id("mat_rad").cast_form_float(&base->surface.radius);
	state.gui->get_element_by_id("mat_ht_amnt").cast_form_float(&base->surface.height);
	state.gui->get_element_by_id("mat_ht_bias").cast_form_float(&base->surface.bias);
}
void sandbox::set_logging(bool active)
{
	if (!active)
		return error_handling::set_callback(nullptr);

	error_handling::set_callback([this](error_handling::details& data)
	{
		set_status(error_handling::get_message_text(data));
	});
}
void sandbox::set_view_model()
{
	set_status("gui view-model setup done");
	if (!state.gui)
		return;

	state.system = state.gui->set_data_model("editor");
	state.entities = state.system->set_array("entities");
	state.materials = state.system->set_array("materials");
	state.models = state.system->set_array("models");
	state.skins = state.system->set_array("skins");
	state.directories = state.system->set_array("directories");
	state.files = state.system->set_array("files");
	state.system->set_boolean("scene_active", false);
	state.system->set_string("sl_status", "");
	state.system->set_string("sl_window", "none");
	state.system->set_string("sl_material", gui::ielement::from_pointer(nullptr));
	state.system->set_string("sl_entity", gui::ielement::from_pointer(nullptr));
	state.system->set_boolean("sl_resource", false);
	state.system->set_boolean("sl_cmp_model", false);
	state.system->set_string("sl_cmp_model_mesh", "");
	state.system->set_boolean("sl_cmp_model_source", false);
	state.system->set_boolean("sl_cmp_model_assigned", false);
	state.system->set_boolean("sl_cmp_skin", false);
	state.system->set_string("sl_cmp_skin_mesh", "");
	state.system->set_boolean("sl_cmp_skin_source", false);
	state.system->set_boolean("sl_cmp_skin_assigned", false);
	state.system->set_integer("sl_cmp_skin_joint", -1);
	state.system->set_integer("sl_cmp_skin_joints", -1);
	state.system->set_boolean("sl_cmp_emitter", false);
	state.system->set_boolean("sl_cmp_soft_body", false);
	state.system->set_boolean("sl_cmp_soft_body_source", false);
	state.system->set_boolean("sl_cmp_decal", false);
	state.system->set_boolean("sl_cmp_skin_animator", false);
	state.system->set_integer("sl_cmp_skin_animator_joint", -1);
	state.system->set_integer("sl_cmp_skin_animator_joints", -1);
	state.system->set_integer("sl_cmp_skin_animator_frame", -1);
	state.system->set_integer("sl_cmp_skin_animator_frames", -1);
	state.system->set_integer("sl_cmp_skin_animator_clip", -1);
	state.system->set_integer("sl_cmp_skin_animator_clips", -1);
	state.system->set_boolean("sl_cmp_key_animator", false);
	state.system->set_integer("sl_cmp_key_animator_frame", -1);
	state.system->set_integer("sl_cmp_key_animator_frames", -1);
	state.system->set_integer("sl_cmp_key_animator_clip", -1);
	state.system->set_integer("sl_cmp_key_animator_clips", -1);
	state.system->set_boolean("sl_cmp_emitter_animator", false);
	state.system->set_boolean("sl_cmp_rigid_body", false);
	state.system->set_boolean("sl_cmp_rigid_body_source", false);
	state.system->set_boolean("sl_cmp_rigid_body_from_source", false);
	state.system->set_boolean("sl_cmp_acceleration", false);
	state.system->set_boolean("sl_cmp_slider_constraint", false);
	state.system->set_boolean("sl_cmp_slider_constraint_entity", false);
	state.system->set_boolean("sl_cmp_free_look", false);
	state.system->set_boolean("sl_cmp_fly", false);
	state.system->set_boolean("sl_cmp_audio_source", false);
	state.system->set_boolean("sl_cmp_audio_source_clip", false);
	state.system->set_float("sl_cmp_audio_source_length", 0.0f);
	state.system->set_boolean("sl_cmp_audio_source_reverb", false);
	state.system->set_boolean("sl_cmp_audio_source_chorus", false);
	state.system->set_boolean("sl_cmp_audio_source_distortion", false);
	state.system->set_boolean("sl_cmp_audio_source_echo", false);
	state.system->set_boolean("sl_cmp_audio_source_flanger", false);
	state.system->set_boolean("sl_cmp_audio_source_frequency_shifter", false);
	state.system->set_boolean("sl_cmp_audio_source_vocal_morpher", false);
	state.system->set_boolean("sl_cmp_audio_source_pitch_shifter", false);
	state.system->set_boolean("sl_cmp_audio_source_ring_modulator", false);
	state.system->set_boolean("sl_cmp_audio_source_autowah", false);
	state.system->set_boolean("sl_cmp_audio_source_compressor", false);
	state.system->set_boolean("sl_cmp_audio_source_equalizer", false);
	state.system->set_boolean("sl_cmp_audio_listener", false);
	state.system->set_boolean("sl_cmp_point_light", false);
	state.system->set_boolean("sl_cmp_spot_light", false);
	state.system->set_boolean("sl_cmp_line_light", false);
	state.system->set_integer("sl_cmp_line_light_cascades", -1);
	state.system->set_boolean("sl_cmp_surface_light", false);
	state.system->set_boolean("sl_cmp_illuminator", false);
	state.system->set_boolean("sl_cmp_camera", false);
	state.system->set_integer("sl_cmp_camera_model", -1);
	state.system->set_integer("sl_cmp_camera_skin", -1);
	state.system->set_integer("sl_cmp_camera_soft_body", -1);
	state.system->set_integer("sl_cmp_camera_decal", -1);
	state.system->set_integer("sl_cmp_camera_emitter", -1);
	state.system->set_integer("sl_cmp_camera_depth", -1);
	state.system->set_integer("sl_cmp_camera_lighting", -1);
	state.system->set_integer("sl_cmp_camera_environment", -1);
	state.system->set_integer("sl_cmp_camera_transparency", -1);
	state.system->set_integer("sl_cmp_camera_sslr", -1);
	state.system->set_integer("sl_cmp_camera_ssli", -1);
	state.system->set_integer("sl_cmp_camera_ssla", -1);
	state.system->set_integer("sl_cmp_camera_motionblur", -1);
	state.system->set_integer("sl_cmp_camera_bloom", -1);
	state.system->set_integer("sl_cmp_camera_dof", -1);
	state.system->set_integer("sl_cmp_camera_tone", -1);
	state.system->set_integer("sl_cmp_camera_glitch", -1);
	state.system->set_integer("sl_cmp_camera_gui", -1);
	state.system->set_boolean("sl_cmp_scriptable", false);
	state.system->set_boolean("sl_cmp_scriptable_source", false);
	state.system->set_callback("set_parent", [this](gui::ievent& event, const variant_list& args)
	{
		gui::ielement target = (Rml::Element*)event.get_pointer("drag_element");
		if (!target.is_valid())
			return;

		entity* child = (entity*)gui::ielement::to_pointer(target.get_attribute("entity"));
		if (!child)
			return;

		if (args.size() == 1)
		{
			entity* base = (entity*)gui::ielement::to_pointer(args[0].get_blob());
			if (base != nullptr)
				child->set_root(base);
			else
				child->set_root(nullptr);
			event.stop_immediate_propagation();
		}
		else if (args.empty())
			child->set_root(nullptr);
	});
	state.system->set_callback("set_entity", [this](gui::ievent& event, const variant_list& args)
	{
		if (args.size() != 1)
			return;

		entity* base = (entity*)gui::ielement::to_pointer(args[0].get_blob());
		if (state.on_entity)
		{
			auto callback = state.on_entity;
			get_entity("__got__", nullptr);

			callback(base);
		}
		else if (selection.entity != base)
			set_selection(inspector_entity, base);
		else
			set_selection(inspector_none);
	});
	state.system->set_callback("set_controls", [this](gui::ievent& event, const variant_list& args)
	{
		if (args.size() == 1)
			state.is_scene_focused = args[0].get_boolean();
	});
	state.system->set_callback("set_menu", [](gui::ievent& event, const variant_list& args)
	{
		if (args.size() != 1)
			return;

		gui::ielement target = event.get_current_element();
		gui::ielement current = event.get_target_element();
		bool toggle = false;

		if (target.get_element() != current.get_element())
		{
			if (event.get_type() != "click")
				return;

			if (current.get_tag_name() != "button")
			{
				if (current.get_tag_name() != "p")
					return;

				toggle = true;
			}
		}

		vector<gui::ielement> tabs = target.query_selector_all(".tab");
		bool enabled = args[0].get_boolean();

		for (auto& tab : tabs)
		{
			if (toggle)
				tab.set_class("transfer", !tab.is_class_set("transfer"));
			else if (enabled)
				tab.set_class("transfer", true);
			else
				tab.set_class("transfer", false);
		}
	});
	state.system->set_callback("set_directory", [this](gui::ievent& event, const variant_list& args)
	{
		set_directory(args[0].get_blob());
	});
	state.system->set_callback("set_file", [this](gui::ievent& event, const variant_list& args)
	{
		if (args.size() != 1)
			return;

		string fResource = args[0].serialize();
		if (fResource.empty() || !state.on_resource)
			return;

		auto callback = state.on_resource;
		get_resource("__got__", nullptr);

		callback(fResource);
	});
	state.system->set_callback("set_material", [this](gui::ievent& event, const variant_list& args)
	{
		if (args.size() != 1)
			return;

		material* base = (material*)gui::ielement::to_pointer(args[0].get_blob());
		set_selection(inspector_material, base);
	});
	state.system->set_callback("save_settings", [this](gui::ievent& event, const variant_list& args)
	{
		scene->configure(scene->get_conf());
	});
	state.system->set_callback("switch_scene", [this](gui::ievent& event, const variant_list& args)
	{
		if (!scene->is_active())
		{
			variant_args args;
			args["type"] = var::string("XML");

			unload_camera();
			content->save<scene_graph>("./editor/cache.xml", scene, args);
			scene->set_active(true);
		}
		else
			this->resource.next_path = "./editor/cache.xml";
	});
	state.system->set_callback("import_model_action", [this](gui::ievent& event, const variant_list& args)
	{
		string from;
		if (!alerts::open("Import mesh", content->get_environment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &from))
			return;

		if (!os::file::is_exists(from.c_str()))
			return;

		processors::model_processor* processor = (processors::model_processor*)content->get_processor<model>();
		if (processor != nullptr)
		{
			stream* file = *os::file::open(from, file_mode::binary_read_only);
			auto target = processor->import(file, state.mesh_import_opts);
			memory::release(file);

			if (target)
			{
				string to;
				if (!alerts::save("Save mesh", content->get_environment(), "*.xml,*.json,*.jsonb", "Serialized mesh (*.xml, *.json, *.jsonb)", &to))
					return;

				variant_args args;
				if (stringify::ends_with(to, ".jsonb"))
					args["type"] = var::string("JSONB");
				else if (stringify::ends_with(to, ".json"))
					args["type"] = var::string("JSON");
				else
					args["type"] = var::string("XML");

				content->save<schema>(to, *target, args);
				memory::release(*target);
				this->activity->message.setup(alert_type::info, "Sandbox", "Mesh was imported");
			}
			else
				this->activity->message.setup(alert_type::error, "Sandbox", "Mesh is unsupported");
		}
		else
			this->activity->message.setup(alert_type::info, "Sandbox", "Mesh cannot be imported");

		this->activity->message.button(alert_confirm::defer, "OK", 1);
		this->activity->message.result(nullptr);
	});
	state.system->set_callback("import_skin_animation_action", [this](gui::ievent& event, const variant_list& args)
	{
		string from;
		if (!alerts::open("Import animation from mesh", content->get_environment(), "*.dae,*.fbx,*.gltf,*.glb,*.blend,*.3d,*.3ds,*.ase,*.obj,*.ifc,*.xgl,*.zgl,*.ply,*.lwo,*.lws,*.lxo,*.stl,*.x,*.ac,*.ms3d,*.mdl,*.md2,.*md3", "", false, &from))
			return;

		if (!os::file::is_exists(from.c_str()))
			return;

		processors::skin_animation_processor* processor = (processors::skin_animation_processor*)content->get_processor<skin_animation>();
		if (processor != nullptr)
		{
			stream* file = *os::file::open(from, file_mode::binary_read_only);
			auto target = processor->import(file, state.mesh_import_opts);
			memory::release(file);

			if (target)
			{
				string to;
				if (!alerts::save("Save animation", content->get_environment(), "*.xml,*.json,*.jsonb", "Serialized skin animation (*.xml, *.json, *.jsonb)", &to))
					return;

				variant_args args;
				if (stringify::ends_with(to, ".jsonb"))
					args["type"] = var::string("JSONB");
				else if (stringify::ends_with(to, ".json"))
					args["type"] = var::string("JSON");
				else
					args["type"] = var::string("XML");

				content->save<schema>(to, *target, args);
				memory::release(*target);
				this->activity->message.setup(alert_type::info, "Sandbox", "Animation was imported");
			}
			else
				this->activity->message.setup(alert_type::error, "Sandbox", "Animation is unsupported");
		}
		else
			this->activity->message.setup(alert_type::info, "Sandbox", "Animation cannot be imported");

		this->activity->message.button(alert_confirm::defer, "OK", 1);
		this->activity->message.result(nullptr);
	});
	state.system->set_callback("remove_cmp", [this](gui::ievent& event, const variant_list& args)
	{
		if (args.size() != 1)
			return;

		if (selection.entity != nullptr)
			selection.entity->remove_component(VI_HASH(args[0].serialize()));
	});
	state.system->set_callback("open_materials", [this](gui::ievent& event, const variant_list& args)
	{
		set_selection(inspector_materials);
	});
	state.system->set_callback("remove_material", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.material != nullptr)
			scene->delete_material(selection.material);
		set_selection(inspector_materials);
	});
	state.system->set_callback("copy_material", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.material != nullptr)
		{
			selection.material = scene->clone_material(selection.material);
			selection.material->set_name(selection.material->get_name() + "*");
		}
	});
	state.system->set_callback("open_settings", [this](gui::ievent& event, const variant_list& args)
	{
		set_selection(inspector_settings);
	});
	state.system->set_callback("add_material", [this](gui::ievent& event, const variant_list& args)
	{
		material* init = scene->add_material();
		init->set_name("Material " + to_string(scene->get_materials_count() + 1));
		set_selection(inspector_material, init);
	});
	state.system->set_callback("import_model", [this](gui::ievent& event, const variant_list& args)
	{
		set_selection(inspector_import_model);
	});
	state.system->set_callback("import_skin_animation", [this](gui::ievent& event, const variant_list& args)
	{
		set_selection(inspector_import_animation);
	});
	state.system->set_callback("export_key_animation", [this](gui::ievent& event, const variant_list& args)
	{
		if (!selection.entity || !selection.entity->get_component<components::key_animator>())
		{
			this->activity->message.setup(alert_type::info, "Sandbox", "Select entity with key animator to export");
			this->activity->message.button(alert_confirm::defer, "OK", 1);
			this->activity->message.result(nullptr);
			return;
		}

		string path;
		if (!alerts::save("Save key animation", content->get_environment(), "*.xml,*.json,*.jsonb", "Serialized key animation (*.xml, *.json, *.jsonb)", &path))
			return;

		schema* result = var::set::object();
		result->key = "key-animation";

		auto* animator = selection.entity->get_component<components::key_animator>();
		heavy_series::pack(result, animator->clips);

		variant_args map;
		if (stringify::ends_with(path, ".jsonb"))
			map["type"] = var::string("JSONB");
		else if (stringify::ends_with(path, ".json"))
			map["type"] = var::string("JSON");
		else
			map["type"] = var::string("XML");

		if (!content->save<schema>(path, result, map))
			this->activity->message.setup(alert_type::error, "Sandbox", "Key animation cannot be saved");
		else
			this->activity->message.setup(alert_type::info, "Sandbox", "Key animation was saved");

		memory::release(result);
		this->activity->message.button(alert_confirm::defer, "OK", 1);
		this->activity->message.result(nullptr);
	});
	state.system->set_callback("import_material", [this](gui::ievent& event, const variant_list& args)
	{
		get_resource("material", [this](const string& file)
		{
			auto result = content->load<material>(file);
			if (!result || !scene->add_material(*result))
				return;

			set_selection(inspector_material, *result);
		});
	});
	state.system->set_callback("deploy_scene", [this](gui::ievent& event, const variant_list& args)
	{
		if (!resource.scene_path.empty())
		{
			this->activity->message.setup(alert_type::warning, "Sandbox", "Editor's state will be flushed before start");
			this->activity->message.button(alert_confirm::escape, "No", 1);
			this->activity->message.button(alert_confirm::defer, "Yes", 2);
			this->activity->message.result([this](int button)
			{
				if (button == 2)
				{
					demo::set_source(resource.scene_path);
					stop();
				}
			});
		}
		else
		{
			this->activity->message.setup(alert_type::error, "Sandbox", "Save the scene to deploy it later");
			this->activity->message.button(alert_confirm::defer, "OK", 1);
			this->activity->message.result(nullptr);
		}
	});
	state.system->set_callback("compile_shaders", [this](gui::ievent& event, const variant_list& args)
	{
		string path = content->get_environment() + "./shaders";
		auto subpath = os::path::resolve(path.c_str());
		if (subpath)
			path = *subpath;
		os::directory::patch(path);

		graphics_device::desc D3D11;
		D3D11.backend = render_backend::d3d11;
		D3D11.cache_directory = path;

		graphics_device::desc OGL;
		OGL.backend = render_backend::ogl;
		OGL.cache_directory = path;

		vector<graphics_device*> devices;
		devices.push_back(renderer->get_backend() == D3D11.backend ? renderer : graphics_device::create(D3D11));
		devices.push_back(renderer->get_backend() == OGL.backend ? renderer : graphics_device::create(OGL));
		this->renderer->add_ref();

		graphics_device::compile_builtin_shaders(devices, [](graphics_device* device, const std::string_view& name, const expects_graphics<shader*>& result) -> bool
		{
			auto get_device_name = [&device]() -> const char*
			{
				switch (device->get_backend())
				{
					case render_backend::d3d11:
						return "d3d11";
					case render_backend::ogl:
						return "opengl";
					default:
						return "unknown";
				}
			};

			if (result)
			{
				VI_INFO("[sandbox] OK compile %s shader %.*s", get_device_name(), (int)name.size(), name.data());
				memory::release(*result);
			}
			else
				VI_ERR("[sandbox] cannot compile %s shader %.*s: %s", get_device_name(), (int)name.size(), name.data(), result.error().what());
			return !!result;
		});
		for (auto* device : devices)
			memory::release(device);
	});
	state.system->set_callback("open_scene", [this](gui::ievent& event, const variant_list& args)
	{
		get_resource("scene", [this](const string& file)
		{
			this->resource.next_path = file;
		});
	});
	state.system->set_callback("close_scene", [this](gui::ievent& event, const variant_list& args)
	{
		string path;
		if (!alerts::save("Save scene", content->get_environment(), "*.xml,*.json,*.jsonb", "Serialized scene (*.xml, *.json, *.jsonb)", &path))
			return;

		variant_args map;
		if (stringify::ends_with(path, ".jsonb"))
			map["type"] = var::string("JSONB");
		else if (stringify::ends_with(path, ".json"))
			map["type"] = var::string("JSON");
		else
			map["type"] = var::string("XML");

		unload_camera();
		content->save<scene_graph>(path, scene, map);
		load_camera();

		if (!scene->is_active())
			scene->set_camera(state.camera);

		this->activity->message.setup(alert_type::info, "Sandbox", "Scene was saved");
		this->activity->message.button(alert_confirm::defer, "OK", 1);
		this->activity->message.result(nullptr);
	});
	state.system->set_callback("update_files", [this](gui::ievent& event, const variant_list& args)
	{
		update_files(resource.current_path);
	});
	state.system->set_callback("cancel_file", [this](gui::ievent& event, const variant_list& args)
	{
		get_resource("", nullptr);
	});
	state.system->set_callback("add_entity", [this](gui::ievent& event, const variant_list& args)
	{
		set_selection(inspector_entity, scene->add_entity());
	});
	state.system->set_callback("remove_entity", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			scene->delete_entity(selection.entity);
			set_selection(inspector_none);
		}
	});
	state.system->set_callback("move_entity", [this](gui::ievent& event, const variant_list& args)
	{
		selection.gizmo = resource.gizmo[selection.move = 0];
		selection.gizmo->SetEditMatrix(state.gizmo.row);
		selection.gizmo->SetDisplayScale(state.gizmo_scale);
		selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	state.system->set_callback("rotate_entity", [this](gui::ievent& event, const variant_list& args)
	{
		selection.gizmo = resource.gizmo[selection.move = 1];
		selection.gizmo->SetEditMatrix(state.gizmo.row);
		selection.gizmo->SetDisplayScale(state.gizmo_scale);
		selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	state.system->set_callback("scale_entity", [this](gui::ievent& event, const variant_list& args)
	{
		selection.gizmo = resource.gizmo[selection.move = 2];
		selection.gizmo->SetEditMatrix(state.gizmo.row);
		selection.gizmo->SetDisplayScale(state.gizmo_scale);
		selection.gizmo->SetLocation(IGizmo::LOCATE_WORLD);
	});
	state.system->set_callback("place_position", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_position(scene->get_camera()->get_entity()->get_transform()->get_position());
			state.gizmo = selection.entity->get_transform()->get_bias();
			get_entity_sync();
		}
	});
	state.system->set_callback("place_rotation", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_rotation(scene->get_camera()->get_entity()->get_transform()->get_rotation());
			state.gizmo = selection.entity->get_transform()->get_bias();

			auto* source = selection.entity->get_component<components::audio_source>();
			if (source != nullptr)
				source->get_sync().direction = -scene->get_camera()->get_entity()->get_transform()->get_rotation().ddirection();

			get_entity_sync();
		}
	});
	state.system->set_callback("place_combine", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_position(scene->get_camera()->get_entity()->get_transform()->get_position());
			selection.entity->get_transform()->set_rotation(scene->get_camera()->get_entity()->get_transform()->get_rotation());
			state.gizmo = selection.entity->get_transform()->get_bias();

			auto* source = selection.entity->get_component<components::audio_source>();
			if (source != nullptr)
				source->get_sync().direction = -scene->get_camera()->get_entity()->get_transform()->get_rotation().ddirection();

			get_entity_sync();
		}
	});
	state.system->set_callback("reset_position", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_position(0);
			get_entity_sync();
		}
	});
	state.system->set_callback("reset_rotation", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_rotation(0);
			get_entity_sync();
		}
	});
	state.system->set_callback("reset_scale", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			selection.entity->get_transform()->set_scale(1);
			get_entity_sync();
		}
	});
	state.system->set_callback("add_cmp_skin_animator", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::skin_animator>();
	});
	state.system->set_callback("add_cmp_key_animator", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::key_animator>();
	});
	state.system->set_callback("add_cmp_emitter_animator", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::emitter_animator>();
	});
	state.system->set_callback("add_cmp_listener", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::audio_listener>();
	});
	state.system->set_callback("add_cmp_source", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::audio_source>();
	});
	state.system->set_callback("add_cmp_reverb", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::reverb());
		}
	});
	state.system->set_callback("add_cmp_chorus", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::chorus());
		}
	});
	state.system->set_callback("add_cmp_distortion", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::distortion());
		}
	});
	state.system->set_callback("add_cmp_echo", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::echo());
		}
	});
	state.system->set_callback("add_cmp_flanger", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::flanger());
		}
	});
	state.system->set_callback("add_cmp_frequency_shifter", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::frequency_shifter());
		}
	});
	state.system->set_callback("add_cmp_vocal_morpher", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::vocal_morpher());
		}
	});
	state.system->set_callback("add_cmp_pitch_shifter", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::pitch_shifter());
		}
	});
	state.system->set_callback("add_cmp_ring_modulator", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::ring_modulator());
		}
	});
	state.system->set_callback("add_cmp_autowah", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::autowah());
		}
	});
	state.system->set_callback("add_cmp_compressor", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::compressor());
		}
	});
	state.system->set_callback("add_cmp_equalizer", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (!source)
				source = selection.entity->add_component<components::audio_source>();

			source->get_source()->add_effect(new effects::equalizer());
		}
	});
	state.system->set_callback("add_cmp_model", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::model>();
	});
	state.system->set_callback("add_cmp_skin", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::skin>();
	});
	state.system->set_callback("add_cmp_emitter", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::emitter>();
	});
	state.system->set_callback("add_cmp_decal", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::decal>();
	});
	state.system->set_callback("add_cmp_point_light", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::point_light>();
	});
	state.system->set_callback("add_cmp_spot_light", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::spot_light>();
	});
	state.system->set_callback("add_cmp_line_light", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::line_light>();
	});
	state.system->set_callback("add_cmp_surface_light", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::surface_light>();
	});
	state.system->set_callback("add_cmp_illuminator", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::illuminator>();
	});
	state.system->set_callback("add_cmp_rigid_body", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::rigid_body>();
	});
	state.system->set_callback("add_cmp_soft_body", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::soft_body>();
	});
	state.system->set_callback("add_cmp_slider_constraint", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::slider_constraint>();
	});
	state.system->set_callback("add_cmp_acceleration", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::acceleration>();
	});
	state.system->set_callback("add_cmp_fly", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::fly>();
	});
	state.system->set_callback("add_cmp_free_look", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::free_look>();
	});
	state.system->set_callback("add_cmp_camera", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::camera>();
	});
	state.system->set_callback("add_cmp_3d_camera", [this](gui::ievent& event, const variant_list& args)
	{
		if (!selection.entity)
			return;

		components::camera* camera = selection.entity->add_component<components::camera>();
		auto* fRenderer = camera->get_renderer();
		fRenderer->add_renderer<renderers::model>();
		fRenderer->add_renderer<renderers::skin>();
		fRenderer->add_renderer<renderers::soft_body>();
		fRenderer->add_renderer<renderers::emitter>();
		fRenderer->add_renderer<renderers::decal>();
		fRenderer->add_renderer<renderers::lighting>();
		fRenderer->add_renderer<renderers::transparency>();
	});
	state.system->set_callback("add_cmp_scriptable", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
			selection.entity->add_component<components::scriptable>();
	});
	state.system->set_callback("add_rndr_model", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::model>();
		}
	});
	state.system->set_callback("add_rndr_skin", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::skin>();
		}
	});
	state.system->set_callback("add_rndr_soft_body", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::soft_body>();
		}
	});
	state.system->set_callback("add_rndr_decal", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::decal>();
		}
	});
	state.system->set_callback("add_rndr_emitter", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::emitter>();
		}
	});
	state.system->set_callback("add_rndr_lighting", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::lighting>();
		}
	});
	state.system->set_callback("add_rndr_transparency", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::transparency>();
		}
	});
	state.system->set_callback("add_rndr_ssli", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::local_illumination>();
		}
	});
	state.system->set_callback("add_rndr_sslr", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::local_reflections>();
		}
	});
	state.system->set_callback("add_rndr_ssla", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::local_ambient>();
		}
	});
	state.system->set_callback("add_rndr_motionblur", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::motion_blur>();
		}
	});
	state.system->set_callback("add_rndr_bloom", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::bloom>();
		}
	});
	state.system->set_callback("add_rndr_dof", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::depth_of_field>();
		}
	});
	state.system->set_callback("add_rndr_tone", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::tone>();
		}
	});
	state.system->set_callback("add_rndr_glitch", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::glitch>();
		}
	});
	state.system->set_callback("add_rndr_gui", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (!source)
				source = selection.entity->add_component<components::camera>();

			source->get_renderer()->add_renderer<renderers::user_interface>();
		}
	});
	state.system->set_callback("up_rndr", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr && args.size() == 1)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (source != nullptr)
				source->get_renderer()->move_renderer(VI_HASH(args[0].serialize()), -1);
		}
	});
	state.system->set_callback("down_rndr", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr && args.size() == 1)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (source != nullptr)
				source->get_renderer()->move_renderer(VI_HASH(args[0].serialize()), 1);
		}
	});
	state.system->set_callback("remove_rndr", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr && args.size() == 1)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (source != nullptr)
				source->get_renderer()->remove_renderer(VI_HASH(args[0].serialize()));
		}
	});
	state.system->set_callback("toggle_rndr", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr && args.size() == 1)
		{
			auto* source = selection.entity->get_component<components::camera>();
			if (source != nullptr)
			{
				auto* fRenderer = source->get_renderer()->get_renderer(VI_HASH(args[0].serialize()));
				if (fRenderer != nullptr)
				{
					fRenderer->active = !fRenderer->active;
					if (fRenderer->active)
					{
						fRenderer->deactivate();
						fRenderer->activate();
					}
				}
			}
		}
	});
	state.system->set_callback("remove_aefx", [this](gui::ievent& event, const variant_list& args)
	{
		if (selection.entity != nullptr && args.size() == 1)
		{
			auto* source = selection.entity->get_component<components::audio_source>();
			if (source != nullptr)
				source->get_source()->remove_effect_by_id(VI_HASH(args[0].serialize()));
		}
	});
}
void sandbox::set_directory(const string& path)
{
	state.files->clear();
	selection.pathname = path;

	vector<std::pair<string, file_entry>> files;
	if (!os::directory::scan(selection.pathname, files) || files.empty())
		return;

	string subpath = path + VI_SPLITTER;
	for (auto& file : files)
	{
		if (file.second.is_directory)
			continue;

		string time = date_time::serialize_local(date_time(std::chrono::seconds(std::max(file.second.creation_time, file.second.last_modified))).current_offset(), date_time::format_compact_time());
		string pathname = subpath + file.first;
		string filename = pathname;
		get_path_name(filename);

		variant_list item;
		item.emplace_back(std::move(var::string(pathname)));
		item.emplace_back(std::move(var::string(filename)));
		item.emplace_back(std::move(var::string(stringify::text("%.2f KB", (double)file.second.size / 1000.0))));
		item.emplace_back(std::move(var::string(time)));
		state.files->add(item);
	}
}
void sandbox::set_selection(inspector window, void* object)
{
	selection.might = nullptr;
	selection.entity = nullptr;
	selection.material = nullptr;
	selection.gizmo = nullptr;
	selection.move = 0;
	selection.window = window;

	switch (window)
	{
		case inspector_none:
			set_status("Selection lost");
			break;
		case inspector_entity:
			set_status("Entity was selected");
			selection.entity = (entity*)object;
			set_metadata(selection.entity);
			break;
		case inspector_material:
			set_status("Material was selected");
			selection.material = (material*)object;
			break;
		case inspector_settings:
			set_status("Settings were selected");
			break;
		case inspector_materials:
			set_status("Materials were selected");
			break;
		case inspector_import_model:
			set_status("Model import was selected");
			break;
		case inspector_import_animation:
			set_status("Animation import was selected");
			break;
	}
}
void sandbox::set_status(const string& status)
{
	state.status = status;
}
void sandbox::set_mutation(entity* parent, const std::string_view& type)
{
	if (!parent)
		return;

	for (size_t i = 0; i < parent->get_childs_count(); i++)
	{
		entity* child = parent->get_child(i);
		scene->mutate(parent, child, type);
		set_mutation(child, type);
	}
}
void sandbox::set_metadata(entity* source)
{
	if (!source)
		return;

	auto* model = source->get_component<components::model>();
	if (state.models != nullptr && model != nullptr)
	{
		state.models->clear();
		if (model->get_drawable())
		{
			for (auto* buffer : model->get_drawable()->meshes)
			{
				variant_list item;
				item.emplace_back(var::string(gui::ielement::from_pointer(buffer)));
				item.emplace_back(var::string(buffer->name.empty() ? "Unknown" : buffer->name));
				state.models->add(item);
			}
		}
	}

	auto* skin = source->get_component<components::skin>();
	if (state.skins != nullptr && skin != nullptr)
	{
		state.skins->clear();
		if (skin->get_drawable())
		{
			for (auto* buffer : skin->get_drawable()->meshes)
			{
				variant_list item;
				item.emplace_back(var::string(gui::ielement::from_pointer(buffer)));
				item.emplace_back(var::string(buffer->name.empty() ? "Unknown" : buffer->name));
				state.skins->add(item);
			}
		}
	}
}
void sandbox::get_path_name(string& path)
{
	int64_t distance = 0;
	if (path.back() == '/' || path.back() == '\\')
		path.erase(path.size() - 1);

	for (size_t i = path.size(); i > 0; i--)
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			distance = (int64_t)i + 1;
			break;
		}
	}

	if (distance > 0)
		path.erase(0, (size_t)distance);
}
void sandbox::get_entity_cell()
{
	if (!state.camera || !get_scene_focus() || (selection.gizmo && selection.gizmo->IsActive()))
		return;

	auto* camera = state.camera->get_component<components::camera>();
	bool want_change = activity->is_key_down_hit(key_code::cursor_left);
	ray cursor = camera->get_cursor_ray();
	float distance = -1.0f;
	entity* current = nullptr;

	for (uint32_t i = 0; i < scene->get_entities_count(); i++)
	{
		entity* value = scene->get_entity(i);
		if (selection.entity == value || value == state.camera)
			continue;

		float far_distance = camera->get_distance(value);
		if (distance > 0.0f && distance < far_distance)
			continue;

		matrix4x4 transform = value->get_box();
		if (camera->ray_test(cursor, transform))
		{
			current = value;
			distance = far_distance;
		}
	}

	if (current != nullptr)
	{
		if (!want_change)
		{
			if (selection.entity != current)
				selection.might = current;
		}
		else
			set_selection(inspector_entity, current);
	}
	else
	{
		selection.might = nullptr;
		if (want_change)
			set_selection(inspector_none);
	}
}
void sandbox::get_entity_sync()
{
	components::rigid_body* rigid_body = selection.entity->get_component<components::rigid_body>();
	if (rigid_body != nullptr)
	{
		rigid_body->set_transform(true);
		if (rigid_body->get_body())
		{
			rigid_body->get_body()->set_angular_velocity(0);
			rigid_body->get_body()->set_linear_velocity(0);
		}
	}

	components::soft_body* soft_body = selection.entity->get_component<components::soft_body>();
	if (soft_body != nullptr)
	{
		soft_body->set_transform(true);
		if (soft_body->get_body())
			soft_body->get_body()->set_velocity(0);
	}
}
void sandbox::get_resource(const string& name, const std::function<void(const string&)>& callback)
{
	if (name == "__got__")
	{
		set_status("Resource selected");

		state.on_resource = nullptr;
		state.filename.clear();
		return;
	}

	if (name.empty() || !callback)
	{
		set_status("Resource selection cancelled");

		state.on_resource = nullptr;
		state.filename.clear();
		return;
	}

	set_status("Awaiting " + name + " resource selection");

	state.on_resource = callback;
	state.filename = name;
}
void sandbox::get_entity(const string& name, const std::function<void(entity*)>& callback)
{
	if (name == "__got__")
	{
		set_status("Entity selected");

		state.draggable = nullptr;
		state.on_entity = nullptr;
		state.target.clear();
		return;
	}

	state.draggable = nullptr;
	if (name.empty() || !callback)
	{
		set_status("Entity selection cancelled");

		state.on_entity = nullptr;
		state.target.clear();
		return;
	}

	set_status("Awaiting " + name + " entity selection");

	state.on_entity = callback;
	state.target = name;
}
bool sandbox::get_scene_focus()
{
	return state.is_scene_focused && !state.gui->is_input_focused();
}
bool sandbox::get_resource_state(const string& name)
{
	return state.filename == name;
}
bool sandbox::get_entity_state(const string& name)
{
	return state.target == name;
}
bool sandbox::get_selection_state()
{
	return selection.entity || selection.material;
}
texture_2d* sandbox::get_icon(entity* value)
{
	if (value->get_component<components::camera>())
		return icons.camera;

	if (value->get_component<components::model>() ||
		value->get_component<components::skin>())
		return icons.mesh;

	if (value->get_component<components::emitter>())
		return icons.emitter;

	if (value->get_component<components::decal>())
		return icons.decal;

	if (value->get_component<components::point_light>() ||
		value->get_component<components::spot_light>() ||
		value->get_component<components::line_light>())
		return icons.light;

	if (value->get_component<components::surface_light>() ||
		value->get_component<components::illuminator>())
		return icons.probe;

	if (value->get_component<components::audio_listener>())
		return icons.listener;

	if (value->get_component<components::audio_source>())
		return icons.source;

	if (value->get_component<components::rigid_body>() ||
		value->get_component<components::soft_body>())
		return icons.body;

	if (value->get_component<components::slider_constraint>() ||
		value->get_component<components::acceleration>())
		return icons.motion;

	if (value->get_component<components::key_animator>() ||
		value->get_component<components::skin_animator>() ||
		value->get_component<components::emitter_animator>())
		return icons.animation;

	return icons.empty;
}
uint64_t sandbox::get_entity_nesting(entity* value)
{
	uint64_t top = 0;
	if (!value)
		return top;

	entity* base = value;
	while ((base = base->get_parent()) != nullptr)
		top++;

	return top;
}
void* sandbox::get_entity_index(entity* value)
{
	if (!value)
		return nullptr;

	if (value->get_parent() != nullptr)
		return (void*)value->get_parent();

	return (void*)value;
}
string sandbox::get_label(entity* value)
{
	if (value->get_component<components::camera>())
		return "[camera]";

	if (value->get_component<components::model>())
		return "[model]";

	if (value->get_component<components::skin>())
		return "[skin]";

	if (value->get_component<components::emitter>())
		return "[emitter]";

	if (value->get_component<components::decal>())
		return "[decal]";

	if (value->get_component<components::point_light>())
		return "[point light]";

	if (value->get_component<components::spot_light>())
		return "[spot light]";

	if (value->get_component<components::line_light>())
		return "[line light]";

	if (value->get_component<components::surface_light>())
		return "[surface light]";

	if (value->get_component<components::illuminator>())
		return "[illuminator]";

	if (value->get_component<components::audio_listener>())
		return "[audio listener]";

	if (value->get_component<components::audio_source>())
		return "[audio source]";

	if (value->get_component<components::rigid_body>())
		return "[rigid body]";

	if (value->get_component<components::soft_body>())
		return "[soft body]";

	if (value->get_component<components::slider_constraint>())
		return "[slider constraint]";

	if (value->get_component<components::acceleration>())
		return "[acceleration]";

	if (value->get_component<components::key_animator>())
		return "[key animator]";

	if (value->get_component<components::skin_animator>())
		return "[skin animator]";

	if (value->get_component<components::emitter_animator>())
		return "[emitter animator]";

	if (value->get_component<components::fly>())
		return "[fly]";

	if (value->get_component<components::free_look>())
		return "[free look]";

	if (value->get_component<components::scriptable>())
		return "[scriptable]";

	return "[empty]";
}
string sandbox::get_name(entity* value)
{
	string result;
	if (value == nullptr)
		return "[empty] unknown";

	if (value->get_name().empty())
		result = get_label(value);
	else
		result = get_label(value) + " " + value->get_name();

	auto* scriptable = value->get_component<components::scriptable>();
	if (scriptable != nullptr && !scriptable->get_source().empty())
	{
		const string& library = scriptable->get_name();
		result += " " + (library.empty() ? "anonymous" : library);
	}
	else if (value->get_name().empty())
		result += " unnamed";

	return result;
}
string sandbox::get_pascal(const string& value)
{
	string result;
	result.reserve(value.size() * 2);

	for (auto& symbol : value)
	{
		if (!result.empty() && isupper(symbol))
			result.append(1, ' ').append(1, tolower(symbol));
		else
			result.append(1, symbol);
	}

	return result;
}
