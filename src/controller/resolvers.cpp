#include "resolvers.h"
#include "../core/sandbox.h"

void resolve_resource(gui::ielement& target, const string& name, const std::function<void(const string&)>& callback, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app || !callback)
		return;

	if (!app->get_resource_state(name) && !changed)
	{
		target.set_inner_html(stringify::text("[ awaiting %s ... ]", name.c_str()));
		app->get_resource(name, [name, target, callback](const string& file)
		{
			gui::ielement copy = target;
			copy.set_inner_html(stringify::text("[ %s %s ]", file.empty() ? "setup" : "change", name.c_str()));
			callback(file);
		});
	}
	else
	{
		target.set_inner_html("");
		app->get_resource("", nullptr);
	}
}
void resolve_entity(gui::ielement& target, const string& name, const std::function<void(entity*)>& callback, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app || !callback)
		return;

	if (!app->get_entity_state(name) && !changed)
	{
		target.set_inner_html("[ awaiting entity ... ]");
		app->get_entity(name, [target, callback](entity* source)
		{
			gui::ielement copy = target;
			copy.set_inner_html(stringify::text("[ %s entity ]", source ? "change" : "setup"));
			callback(source);
		});
	}
	else
	{
		target.set_inner_html("");
		app->get_entity("", nullptr);
	}
}
void resolve_texture_2d(gui::context* ui, const string& id, bool assigned, const std::function<void(texture_2d*)>& callback, bool changed)
{
	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!assigned)
		{
			resolve_resource(source, "texture", [callback](const string& file)
			{
				callback(sandbox::get()->content->load<vitex::graphics::texture_2d>(file).or_else(nullptr));
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup texture ]");
			callback(nullptr);
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s texture ]", assigned ? "change" : "setup"));
	}
}
void resolve_key_code(gui::context* ui, const string& id, key_map* output, bool changed)
{
	sandbox* app = ((sandbox*)sandbox::get());
	if (!app)
		return;

	gui::ielement source = ui->get_element_by_id(id);
	if (output->normal && !changed)
	{
		if (source.get_inner_html().empty())
			source.set_inner_html("[ awaiting input ... ]");

		app->state.is_captured = true;
		if (source.is_active())
		{
			app->activity->capture_key_map(nullptr);
			output->normal = false;
			source.set_inner_html("");
		}
		else if (!source.is_hovered() && app->activity->capture_key_map(output))
		{
			output->normal = false;
			source.set_inner_html("");
		}
	}
	else
	{
		auto key_code = video::get_key_code_as_literal(output->key);
		auto key_mod = video::get_key_mod_as_literal(output->mod);
		if (source.get_inner_html().empty())
		{
			if (!key_code.empty() && !key_mod.empty())
				source.set_inner_html(stringify::text("%s + %s", key_mod.data(), key_code.data()));
			else if (!key_code.empty() && key_mod.empty())
				source.set_inner_html(stringify::text("%s", key_code.data()));
			else if (key_code.empty() && !key_mod.empty())
				source.set_inner_html(stringify::text("%s", key_mod.data()));
			else
				source.set_inner_html("[ none ]");
		}

		if (source.is_active())
		{
			source.set_inner_html("");
			output->normal = true;
		}
	}
}
bool resolve_color4(gui::context* ui, const string& id, vitex::trigonometry::vector4* output)
{
	if (!ui->get_element_by_id(id).cast_form_color(output, true))
		return false;

	ui->get_element_by_id(id + "_color").set_property("background-color", stringify::text("rgb(%u, %u, %u, %u)", (unsigned int)(output->x * 255.0f), (unsigned int)(output->y * 255.0f), (unsigned int)(output->z * 255.0f), (unsigned int)(output->w * 255.0f)));
	return true;
}
bool resolve_color3(gui::context* ui, const string& id, vitex::trigonometry::vector3* output)
{
	vitex::trigonometry::vector4 color = *output;
	if (!ui->get_element_by_id(id).cast_form_color(&color, false))
		return false;

	*output = color;
	ui->get_element_by_id(id + "_color").set_property("background-color", stringify::text("rgb(%u, %u, %u)", (unsigned int)(output->x * 255.0f), (unsigned int)(output->y * 255.0f), (unsigned int)(output->z * 255.0f)));
	return true;
}
void resolve_model(gui::context* ui, const string& id, components::model* output, bool changed)
{
	static components::model* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_drawable())
		{
			resolve_resource(source, "model", [output](const string& file)
			{
				auto* app = ((sandbox*)sandbox::get());
				auto instance = app->content->load<vitex::layer::model>(file);
				output->set_drawable(instance.or_else(nullptr));
				app->set_metadata(output->get_entity());
				if (instance)
					memory::release(*instance);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup model ]");
			output->set_drawable(nullptr);
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s model ]", output->get_drawable() ? "change" : "setup"));
	}
}
void resolve_skin(gui::context* ui, const string& id, components::skin* output, bool changed)
{
	static components::skin* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_drawable())
		{
			resolve_resource(source, "skin", [output](const string& file)
			{
				auto* app = ((sandbox*)sandbox::get());
				auto instance = app->content->load<vitex::layer::skin_model>(file);
				output->set_drawable(instance.or_else(nullptr));
				app->set_metadata(output->get_entity());
				if (instance)
					memory::release(*instance);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup skin ]");
			output->set_drawable(nullptr);
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s skin ]", output->get_drawable() ? "change" : "setup"));
	}
}
void resolve_soft_body(gui::context* ui, const string& id, components::soft_body* output, bool changed)
{
	static components::soft_body* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_body())
		{
			resolve_resource(source, "soft body", [output](const string& file)
			{
				output->load(file, 0.0f);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup soft body ]");
			output->clear();
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s soft body ]", output->get_body() ? "change" : "setup"));
	}
}
void resolve_rigid_body(gui::context* ui, const string& id, components::rigid_body* output, bool changed)
{
	static components::rigid_body* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_body())
		{
			resolve_resource(source, "rigid body", [output](const string& file)
			{
				output->load(file, 0.0f, 0.0f);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup rigid body ]");
			output->clear();
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s rigid body ]", output->get_body() ? "change" : "setup"));
	}
}
void resolve_slider_constraint(gui::context* ui, const string& id, components::slider_constraint* output, bool ghost, bool linear, bool changed)
{
	static components::slider_constraint* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_constraint())
		{
			resolve_entity(source, "slider constraint", [output, ghost, linear](entity* source)
			{
				output->load(source, ghost, linear);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup slider constraint ]");
			output->clear();
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_entity("", nullptr);
		source.set_inner_html(stringify::text("[ %s slider constraint ]", output->get_constraint() ? "change" : "setup"));
	}
}
void resolve_skin_animator(gui::context* ui, const string& id, components::skin_animator* output, bool changed)
{
	static components::skin_animator* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (output->get_path().empty())
		{
			resolve_resource(source, "skin animation", [output](const string& file)
			{
				auto instance = sandbox::get()->content->load<skin_animation>(file);
				output->set_animation(instance.or_else(nullptr));
				if (instance)
					memory::release(*instance);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup skin animation ]");
			output->set_animation(nullptr);
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s skin animation ]", output->get_path().empty() ? "change" : "setup"));
	}
}
void resolve_key_animator(gui::context* ui, const string& id, components::key_animator* output, bool changed)
{
	static components::key_animator* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (output->get_path().empty())
		{
			resolve_resource(source, "key animation", [output](const string& file)
			{
				output->load_animation(file);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup key animation ]");
			output->clear_animation();
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s key animation ]", output->get_path().empty() ? "change" : "setup"));
	}
}
void resolve_audio_source(gui::context* ui, const string& id, components::audio_source* output, bool changed)
{
	static components::audio_source* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (!output->get_source()->get_clip())
		{
			resolve_resource(source, "audio clip", [output](const string& file)
			{
				auto instance = sandbox::get()->content->load<audio_clip>(file);
				output->get_source()->set_clip(instance.or_else(nullptr));
				if (instance)
					memory::release(*instance);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup audio clip ]");
			output->get_source()->set_clip(nullptr);
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s audio clip ]", output->get_source()->get_clip() ? "change" : "setup"));
	}
}
void resolve_scriptable(gui::context* ui, const string& id, components::scriptable* output, bool changed)
{
	static components::scriptable* last = nullptr;
	if (last != output)
	{
		last = output;
		changed = true;
	}

	gui::ielement source = ui->get_element_by_id(id);
	if (source.is_active() && !changed)
	{
		if (output->get_source().empty())
		{
			resolve_resource(source, "script", [output](const string& file)
			{
				output->load_source(components::scriptable::source_type::resource, file);
			}, changed);
		}
		else
		{
			source.set_inner_html("[ setup script ]");
			output->unload_source();
		}
	}
	else if (source.get_inner_html().empty() || changed)
	{
		((sandbox*)sandbox::get())->get_resource("", nullptr);
		source.set_inner_html(stringify::text("[ %s script ]", output->get_source().empty() ? "change" : "setup"));
	}
}
