#pragma once

// aux
mat m_scale;
mat m_position;
mat m_rotation;
mat m_model;
mat m_normal;
mat m_mvp;

shader* sh;
shader* last_shader;
cam* cm;

i32 lasttex = -1;
i32 lastvbo0 = -1;
i32 lastvbo1 = -1;
i32 lastvbo2 = -1;
f32 lasttime = -1;
size lastresolution = { 0, 0 };
vec3 lastreference = { 0, 0, 0 };

f32 r_time = 0;
f64 r_elapsed = 0;

/////

void renderer_unprepare()
{
	glUseProgram(0); // Unbind shader program
}

void renderer_camera(cam* camera)
{
	if (cm == camera)
		return;

	cm = camera;
}

void renderer_shader(shader* shader)
{
	if (sh == shader || shader == NULL)
		return;

	sh = shader;
}

void renderer_change_shader(shader* shader)
{
	if (last_shader == shader || shader == NULL)
		return;

	last_shader = shader;

	lasttex = -1;
	lastvbo0 = -1;
	lastvbo1 = -1;
	lastvbo2 = -1;

    glUseProgram(shader->id);

    if (shader->vertex_location != -1)
    	glEnableVertexAttribArray(shader->vertex_location);

    if (shader->texture_location != -1)
    	glEnableVertexAttribArray(shader->texture_location);

    if (shader->normal_location != -1)
    	glEnableVertexAttribArray(shader->normal_location);
}

void renderer_clear()
{	
	r_time += (now_ms() - r_elapsed) / 1000.f;
	r_elapsed = now_ms();

    glClearColor(
        (f32)bg.r / 255,
		(f32)bg.g / 255,
		(f32)bg.b / 255,
		(f32)bg.a / 255);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // tex for example might have changed (eg. label)
	lasttex = -1;
	lastvbo0 = -1;
	lastvbo1 = -1;
	lastvbo2 = -1;

	lastresolution = F2_ZERO;
	lastreference = F3_ZERO;
}

void renderer_free()
{
	renderer_unprepare();

	cm = NULL;
	sh = NULL;
}

void renderer_alloc()
{
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_BLEND);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE); // multisample...

	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);    

    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

    r_elapsed = now_ms();
	renderer_clear();
}

void model_render(model* model)
{
    if (! model->visible)
        return;

    renderer_change_shader(sh);
    
    if (lasttex != model->texture_id)
    {
    	glBindTexture(GL_TEXTURE_2D, model->texture_id);
    	lasttex = model->texture_id;
    }    

    if (! f3_equals(model->scale, F3_ZERO) && ! f3_equals(model->scale, F3_ONE))
        m_scale = mat_scale(model->scale);
    else
        m_scale = MAT_ONE;

    m_position = mat_transpose(mat_translate(model->pos));
    m_rotation = mat_rotate_vec(model->rot);

	m_model = MAT_ONE;
	m_model = mat_mul(m_model, m_scale);
	m_model = mat_mul(m_model, m_rotation);
	m_model = mat_mul(m_model, m_position);

    m_normal = mat_transpose(mat_invert(m_model));
	    
    // Calculate model-view-projection matrix (MVP)
    m_mvp = mat_mul(m_model, cm->m_view);
    m_mvp = mat_mul(m_mvp, cm->m_projection);

    if (lastvbo0 != model->vbo[0])
    {
		glBindBuffer(GL_ARRAY_BUFFER, model->vbo[0]);
		glVertexAttribPointer(sh->vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

		lastvbo0 = model->vbo[0];
    }
	    
	if (lastvbo1 != model->vbo[1])
	{
		glBindBuffer(GL_ARRAY_BUFFER, model->vbo[1]);
		glVertexAttribPointer(sh->texture_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

		lastvbo1 = model->vbo[1];
	}
	    
	if (sh->normal_location != -1)
	{
		if (lastvbo2 != model->vbo[2])
		{
		    glBindBuffer(GL_ARRAY_BUFFER, model->vbo[2]);
		    glVertexAttribPointer(sh->normal_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

		    lastvbo2 = model->vbo[2];
		}
	}

	// Model, View, Proj matrix
	if (sh->model_matrix != -1)
		glUniformMatrix4fv(sh->model_matrix, 1, false, (f32*)&m_model);

	if (sh->normal_matrix != -1)
		glUniformMatrix4fv(sh->normal_matrix, 1, false, (f32*)&m_normal);

	if (sh->view_matrix != -1)
		glUniformMatrix4fv(sh->view_matrix, 1, false, (f32*)&cm->m_view);

	if (sh->mvp_matrix != -1)
		glUniformMatrix4fv(sh->mvp_matrix, 1, false, (f32*)&m_mvp);

	// Lights Position/Power
	if (sh->light_location != -1)
	    glUniform4fv(sh->light_location, 1, (f32*)&cm->light);

	// POV Position
	if (sh->pov_location != -1)
	    glUniform3fv(sh->pov_location, 1, (f32*)&cm->pov);

	// Time Position
	if (sh->time_location != -1)
	{
		if (lasttime != r_time)
		{
			glUniform1f(sh->time_location, r_time);

			lasttime = r_time;
		}
	}

	// Resolution Position
	if (sh->resolution_location != -1)
	{
		if (!f2_equals(lastresolution, screen))
		{
			glUniform2f(sh->resolution_location, screen.x, screen.y);

			lastresolution = screen;
		}
	}

	// Reference Position
	if (sh->reference_location != -1)
	{
		if (!f3_equals(lastreference, cm->reference))
		{
			glUniform3fv(sh->reference_location, 1, (f32*)&cm->reference);

			lastreference = cm->reference;
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, model->vertex_count);
}

void render_shader_fs()
{
	renderer_change_shader(sh);

    if (lastvbo0 != m_quad->vbo[0])
    {
		glBindBuffer(GL_ARRAY_BUFFER, m_quad->vbo[0]);
		glVertexAttribPointer(sh->vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);

		lastvbo0 = m_quad->vbo[0];
    }

	// Time Position
	if (sh->time_location != -1)
	{
		if (lasttime != r_time)
		{
			glUniform1f(sh->time_location, r_time);

			lasttime = r_time;
		}	    
	}

	// Resolution Position
	if (sh->resolution_location != -1)
	{
		if (!f2_equals(lastresolution, window_size))
		{
			glUniform2f(sh->resolution_location, window_size.x, window_size.y);

			lastresolution = window_size;
		}
	}

	// Reference Position
	if (sh->reference_location != -1)
	{
		if (!f3_equals(lastreference, cm->reference))
		{
			glUniform3fv(sh->reference_location, 1, (f32*)&cm->reference);

			lastreference = cm->reference;
		}

	    // Calculate model-view-projection matrix (MVP)
	    m_mvp = mat_mul(cm->m_view, cm->m_projection);

		if (sh->mvp_matrix != -1)
			glUniformMatrix4fv(sh->mvp_matrix, 1, false, (f32*)&m_mvp);
	}	    


	glDrawArrays(GL_TRIANGLES, 0, m_quad->vertex_count);
}

void renderer_nodepth()
{
	glDisable(GL_DEPTH_TEST);
}

void renderer_depth()
{
	glEnable(GL_DEPTH_TEST);
}