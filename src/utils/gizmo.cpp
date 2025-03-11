#include "gizmo.h"
#include "../core/sandbox.h"

void CGizmoTransformRender::DrawCircle(const tvector3& orig, float r, float g, float b, const tvector3& vtx, const tvector3& vty)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(matrix4x4::create(app->state.gizmo.position(), app->state.gizmo.rotation_euler()) * view_projection);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos((2 * ZPI / 50) * (float)i);
		vt += vty * sin((2 * ZPI / 50) * (float)i);

		app->renderer->im_emit();
		app->renderer->im_position(vt.x, vt.y, vt.z);
		app->renderer->im_color(r, g, b, 1);
	}

	app->renderer->im_end();
}
void CGizmoTransformRender::DrawCircleHalf(const tvector3& orig, float r, float g, float b, const tvector3& vtx, const tvector3& vty, tplane& camPlan)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(matrix4x4::create(app->state.gizmo.position(), app->state.gizmo.rotation_euler()) * view_projection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30) * i);
		vt += vty * sin((ZPI / 30) * i);

		if (camPlan.DotNormal(vt) > 0)
		{
			app->renderer->im_emit();
			app->renderer->im_position(vt.x, vt.y, vt.z);
			app->renderer->im_color(r, g, b, 1);
		}
	}

	app->renderer->im_end();
	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(matrix4x4::create(app->state.gizmo.position(), -1, app->state.gizmo.rotation_euler()) * view_projection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30) * (float)i);
		vt += vty * sin((ZPI / 30) * (float)i);

		if (camPlan.DotNormal(vt) > 0)
		{
			app->renderer->im_emit();
			app->renderer->im_position(vt.x, vt.y, vt.z);
			app->renderer->im_color(r, g, b, 1);
		}
	}

	app->renderer->im_end();
}
void CGizmoTransformRender::DrawAxis(const tvector3& orig, const tvector3& axis, const tvector3& vtx, const tvector3& vty, float fct, float fct2, const tvector4& col)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_list);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_position(orig.x, orig.y, orig.z);
	app->renderer->im_color(col.x, col.y, col.z, 1);
	app->renderer->im_emit();
	app->renderer->im_position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	app->renderer->im_color(col.x, col.y, col.z, 1);
	app->renderer->im_end();
	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::triangle_strip);
	app->renderer->im_transform(view_projection);

	for (uint32_t i = 0; i <= 30; i++)
	{
		tvector3 pt;
		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += axis * fct2;
		pt += orig;

		app->renderer->im_emit();
		app->renderer->im_position(pt.x, pt.y, pt.z);
		app->renderer->im_color(col.x, col.y, col.z, 1);

		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += axis * fct2;
		pt += orig;

		app->renderer->im_emit();
		app->renderer->im_position(pt.x, pt.y, pt.z);
		app->renderer->im_color(col.x, col.y, col.z, 1);
		app->renderer->im_emit();
		app->renderer->im_position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
		app->renderer->im_color(col.x, col.y, col.z, 1);
	}

	app->renderer->im_end();
}
void CGizmoTransformRender::DrawCamem(const tvector3& orig, const tvector3& vtx, const tvector3& vty, float ng)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::triangle_strip);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_color(1, 1, 0, 0.5f);
	app->renderer->im_position(orig.x, orig.y, orig.z);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50) * i);
		vt += vty * sin(((ng) / 50) * i);
		vt += orig;

		app->renderer->im_emit();
		app->renderer->im_color(1, 1, 0, 0.5f);
		app->renderer->im_position(vt.x, vt.y, vt.z);
	}

	app->renderer->im_emit();
	app->renderer->im_color(1, 1, 0, 0.5f);
	app->renderer->im_position(orig.x, orig.y, orig.z);
	app->renderer->im_end();
	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_color(1, 1, 0.2F, 1);
	app->renderer->im_position(orig.x, orig.y, orig.z);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50) * i);
		vt += vty * sin(((ng) / 50) * i);
		vt += orig;

		app->renderer->im_emit();
		app->renderer->im_color(1, 1, 0.2F, 1);
		app->renderer->im_position(vt.x, vt.y, vt.z);
	}

	app->renderer->im_emit();
	app->renderer->im_color(1, 1, 0.2F, 1);
	app->renderer->im_position(orig.x, orig.y, orig.z);
	app->renderer->im_end();
}
void CGizmoTransformRender::DrawQuadAxis(const tvector3& orig, const tvector3& axis, const tvector3& vtx, const tvector3& vty, float fct, float fct2, const tvector4& col)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_list);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_position(orig.x, orig.y, orig.z);
	app->renderer->im_color(col.x, col.y, col.z, 1);
	app->renderer->im_emit();
	app->renderer->im_position(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	app->renderer->im_color(col.x, col.y, col.z, 1);
	app->renderer->im_end();
}
void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	tvector3 pts[4];
	pts[0] = orig;
	pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisU + axisV) * size;
	pts[3] = orig + (axisV * size);

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::triangle_strip);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_end();
	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_end();
}
void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
	auto* app = ((sandbox*)sandbox::get());
	if (!app->scene)
		return;

	matrix4x4 view_projection = ((components::camera*)app->scene->get_camera())->get_view_projection();

	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::triangle_strip);
	app->renderer->im_transform(view_projection);

	tvector3 pts[3];
	pts[0] = orig;
	pts[1] = (axisU);
	pts[2] = (axisV);
	pts[1] *= size;
	pts[2] *= size;
	pts[1] += orig;
	pts[2] += orig;

	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0, 0.5f);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_end();
	app->renderer->im_begin();
	app->renderer->im_topology(primitive_topology::line_strip);
	app->renderer->im_transform(view_projection);
	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_emit();
	app->renderer->im_position(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		app->renderer->im_color(1, 1, 0.2f, 1);
	else
		app->renderer->im_color(1, 1, 1, 0.6f);

	app->renderer->im_end();
}
