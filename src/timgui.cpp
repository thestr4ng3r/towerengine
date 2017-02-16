
#ifdef TOWERENGINE_ENABLE_IMGUI

#include <imgui.h>
#include "towerengine.h"
#include "resources.h"

namespace tImGui
{
	void Render(ImDrawData *draw_data);

	class Shader: public tShader
	{
		private:
			GLint vertex_transform_uniform;

			GLint sprite_coords_uniform;

			GLint tex_uniform;

		public:
			static const int uv_attribute = 1;
			static const int color_attribute = 2;

			static const int tex_unit = 0;

			Shader(void)
			{
				InitShader(resources_get("imgui_shader.vert"), resources_get("imgui_shader.frag"), "ImGui Shader");
				glBindAttribLocation(program, vertex_attribute, "vertex_attr");
				glBindAttribLocation(program, uv_attribute, "uv_attr");
				glBindAttribLocation(program, color_attribute, "color_attr");
				LinkProgram();

				vertex_transform_uniform = GetUniformLocation("vertex_transform_uni");

				tex_uniform = GetUniformLocation("tex_uni");

				Bind();
				glUniform1i(tex_uniform, 0);
			}

			void SetScreenSize(int width, int height)
			{
				glUniform2f(vertex_transform_uniform, 2.0f / (float)width, -2.0f / (float)height);
			}
	};



	Shader *shader;

	GLuint vbo;
	GLuint vao;
	GLuint ibo;

	GLuint font_tex;

	void Init()
	{
		ImGuiIO &io = ImGui::GetIO();

		shader = new Shader();
		shader->Bind();

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(Shader::vertex_attribute);
		glEnableVertexAttribArray(Shader::uv_attribute);
		glEnableVertexAttribArray(Shader::color_attribute);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		glVertexAttribPointer(Shader::vertex_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (const void *)OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(Shader::uv_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (const void *)OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(Shader::color_attribute, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (const void *)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF




		unsigned char *data;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

		glGenTextures(1, &font_tex);
		glBindTexture(GL_TEXTURE_2D, font_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		io.Fonts->TexID = (void *)(intptr_t)font_tex;


		io.RenderDrawListsFn = Render;
	}

	void Shutdown()
	{
		delete shader;

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &ibo);

		glDeleteTextures(1, &font_tex);

		ImGui::Shutdown();
	}

	void BeginFrame(float delta_time, int width, int height, float width_scale, float height_scale)
	{
		ImGuiIO &io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)width, (float)height);
		io.DisplayFramebufferScale = ImVec2(width_scale, height_scale);
		io.DeltaTime = delta_time;

		// TODO: mouse
		io.MousePos = ImVec2(-1, -1);

		ImGui::NewFrame();
	}

	void Render(ImDrawData *draw_data)
	{
		ImGuiIO &io = ImGui::GetIO();

		int width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);

		glActiveTexture(GL_TEXTURE0 + Shader::tex_unit);

		glViewport(0, 0, width, height);

		shader->Bind();
		shader->SetScreenSize(width, height);

		glBindVertexArray(vao);

		for(int i=0; i<draw_data->CmdListsCount; i++)
		{
			const ImDrawList *cmd_list = draw_data->CmdLists[i];
			const ImDrawIdx *idx_buffer_offset = 0;

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

			for(int j=0; j<cmd_list->CmdBuffer.Size; j++)
			{
				const ImDrawCmd *cmd = &cmd_list->CmdBuffer[j];

				if(cmd->UserCallback)
				{
					cmd->UserCallback(cmd_list, cmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd->TextureId);
					glScissor((int)cmd->ClipRect.x,
							  height - (int)cmd->ClipRect.w,
							  (int)(cmd->ClipRect.z - cmd->ClipRect.x),
							  (int)(cmd->ClipRect.w - cmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, cmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}

				idx_buffer_offset += cmd->ElemCount;
			}
		}

		glDisable(GL_SCISSOR_TEST);
	}
}

#endif