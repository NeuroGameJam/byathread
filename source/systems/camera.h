#pragma once

void cam_free(cam* c)
{
	free(c);
}

void cam_update(cam* c)
{
    c->m_view = mat_look(
					c->pov,
					c->look,
					(vec3){ 0.0f, 1.0f, 0.0f });
  
      
    if (! c->ortho)
    	c->m_projection = mat_perspective(
                            c->fov,
                            ratio,
                            c->nnear,
                            c->nfar);
    else
    	c->m_projection = mat_ortho(
                            -c->fov,
                            c->fov,
                            -c->fov / ratio,
                            c->fov / ratio,
                            c->nnear,
                            c->nfar);

    /*
    debug("pov %.2f, %.2f %.2f  - fov %.2f\n",
    	c->pov.x, c->pov.y, c->pov.z,
    	c->fov
    	);
    */
}

vec cam_unproject(cam* c, const vec3 target)
{
	mat aux = mat_mul(c->m_view, c->m_projection);
	aux = mat_mul(mat_transpose(mat_translate(target)), aux);

	return (vec) { aux.wx / aux.ww, aux.wy / aux.ww };
}

cam* cam_alloc()
{
	cam* c = calloc(1, sizeof(cam));

	c->ortho = true;

	c->fov = 30;
	c->pov = (vec3) { 0, 15, -25 };	
	c->look = F3_ZERO;

	c->nfar = 100.f;
	c->nnear = .5f;

	c->light.pos = (vec3) { 0, -1, -1 };
	c->light.power = 250;

	cam_update(c);

	return c;
}
