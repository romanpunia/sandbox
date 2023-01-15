#include "gizmo.h"
#include "../core/sandbox.h"

void CGizmoTransformRender::DrawCircle(const tvector3 &orig, float r, float g, float b, const tvector3 &vtx, const tvector3 &vty)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(Matrix4x4::Create(App->State.Gizmo.Position(), App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos((2 * ZPI / 50) * (float)i);
		vt += vty * sin((2 * ZPI / 50) * (float)i);

		App->Renderer->ImEmit();
		App->Renderer->ImPosition(vt.x, vt.y, vt.z);
		App->Renderer->ImColor(r, g, b, 1);
	}

	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawCircleHalf(const tvector3 &orig, float r, float g, float b, const tvector3 &vtx, const tvector3 &vty, tplane &camPlan)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(Matrix4x4::Create(App->State.Gizmo.Position(), App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30)*i);
		vt += vty * sin((ZPI / 30)*i);

		if (camPlan.DotNormal(vt) > 0)
		{
			App->Renderer->ImEmit();
			App->Renderer->ImPosition(vt.x, vt.y, vt.z);
			App->Renderer->ImColor(r, g, b, 1);
		}
	}

	App->Renderer->ImEnd();
	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(Matrix4x4::Create(App->State.Gizmo.Position(), -1, App->State.Gizmo.Rotation()) * ViewProjection);

	for (uint32_t i = 0; i < 30; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI / 30) * (float)i);
		vt += vty * sin((ZPI / 30) * (float)i);

		if (camPlan.DotNormal(vt) > 0)
		{
			App->Renderer->ImEmit();
			App->Renderer->ImPosition(vt.x, vt.y, vt.z);
			App->Renderer->ImColor(r, g, b, 1);
		}
	}

	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx, const tvector3 &vty, float fct, float fct2, const tvector4 &col)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_List);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);
	App->Renderer->ImColor(col.x, col.y, col.z, 1);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	App->Renderer->ImColor(col.x, col.y, col.z, 1);
	App->Renderer->ImEnd();
	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Triangle_Strip);
	App->Renderer->ImTransform(ViewProjection);

	for (uint32_t i = 0; i <= 30; i++)
	{
		tvector3 pt;
		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)i) * fct;
		pt += axis * fct2;
		pt += orig;

		App->Renderer->ImEmit();
		App->Renderer->ImPosition(pt.x, pt.y, pt.z);
		App->Renderer->ImColor(col.x, col.y, col.z, 1);

		pt = vtx * cos(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += vty * sin(((2 * ZPI) / 30.0f) * (float)(i + 1)) * fct;
		pt += axis * fct2;
		pt += orig;

		App->Renderer->ImEmit();
		App->Renderer->ImPosition(pt.x, pt.y, pt.z);
		App->Renderer->ImColor(col.x, col.y, col.z, 1);
		App->Renderer->ImEmit();
		App->Renderer->ImPosition(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
		App->Renderer->ImColor(col.x, col.y, col.z, 1);
	}

	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawCamem(const tvector3 &orig, const tvector3 &vtx, const tvector3 &vty, float ng)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Triangle_Strip);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImColor(1, 1, 0, 0.5f);
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50)*i);
		vt += vty * sin(((ng) / 50)*i);
		vt += orig;

		App->Renderer->ImEmit();
		App->Renderer->ImColor(1, 1, 0, 0.5f);
		App->Renderer->ImPosition(vt.x, vt.y, vt.z);
	}

	App->Renderer->ImEmit();
	App->Renderer->ImColor(1, 1, 0, 0.5f);
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);
	App->Renderer->ImEnd();
	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImColor(1, 1, 0.2F, 1);
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);

	for (uint32_t i = 0; i <= 50; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng) / 50)*i);
		vt += vty * sin(((ng) / 50)*i);
		vt += orig;

		App->Renderer->ImEmit();
		App->Renderer->ImColor(1, 1, 0.2F, 1);
		App->Renderer->ImPosition(vt.x, vt.y, vt.z);
	}

	App->Renderer->ImEmit();
	App->Renderer->ImColor(1, 1, 0.2F, 1);
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);
	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawQuadAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx, const tvector3 &vty, float fct, float fct2, const tvector4 &col)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_List);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(orig.x, orig.y, orig.z);
	App->Renderer->ImColor(col.x, col.y, col.z, 1);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(orig.x + axis.x, orig.y + axis.y, orig.z + axis.z);
	App->Renderer->ImColor(col.x, col.y, col.z, 1);
	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3 &axisV)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	tvector3 pts[4];
	pts[0] = orig;
	pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisU + axisV) * size;
	pts[3] = orig + (axisV * size);

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Triangle_Strip);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEnd();
	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[1].x, pts[1].y, pts[1].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[2].x, pts[2].y, pts[2].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[3].x, pts[3].y, pts[3].z);

	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEnd();
}
void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
	auto* App = ((Sandbox*)Sandbox::Get());
	if (!App->Scene)
		return;

	Matrix4x4 ViewProjection = ((Components::Camera*)App->Scene->GetCamera())->GetViewProjection();

	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Triangle_Strip);
	App->Renderer->ImTransform(ViewProjection);

	tvector3 pts[3];
	pts[0] = orig;
	pts[1] = (axisU);
	pts[2] = (axisV);
	pts[1] *= size;
	pts[2] *= size;
	pts[1] += orig;
	pts[2] += orig;

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0, 0.5f);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEnd();
	App->Renderer->ImBegin();
	App->Renderer->ImTopology(PrimitiveTopology::Line_Strip);
	App->Renderer->ImTransform(ViewProjection);
	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[1].x, pts[1].y, pts[1].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[2].x, pts[2].y, pts[2].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEmit();
	App->Renderer->ImPosition(pts[0].x, pts[0].y, pts[0].z);
	if (!bSelected)
		App->Renderer->ImColor(1, 1, 0.2f, 1);
	else
		App->Renderer->ImColor(1, 1, 1, 0.6f);

	App->Renderer->ImEnd();
}
