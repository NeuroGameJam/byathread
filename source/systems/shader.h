#pragma once

void internal_gpu_debug(u16 context)
{
    i32 length;
    glGetShaderiv(context, GL_INFO_LOG_LENGTH, &length);
 
    char* msg = (char*)malloc(length+1);
 
    glGetShaderInfoLog(context, length, NULL, msg);

    msg[length] = 0x0;

    debug("ERROR %s\n", msg);

    free(msg);    
}

u16 internal_load_shader_program(const char* shader, const char* fragment)
{    
    u16 program = 0;
    
    GLuint vertexShader;
    GLuint fragmentShader;
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char *pvs = shader;
    const char *pfs = fragment;
    
    glShaderSource(vertexShader, 1, &pvs, 0);
    glShaderSource(fragmentShader, 1, &pfs, 0);
    
    GLint success = 0;
    
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    
    if (success != GL_TRUE)
    {
        debug("[VSHDR ID %i] Failed to compile vertex shader...\n", vertexShader);        
        internal_gpu_debug(vertexShader);
    }
    else
    {
        debug("[VSHDR ID %i] Vertex shader compiled successfully\n", vertexShader);
    }
    
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    
    if (success != GL_TRUE)
    {
        debug("[FSHDR ID %i] Failed to compile fragment shader...\n", fragmentShader);
        internal_gpu_debug(fragmentShader);
    }
    else debug("[FSHDR ID %i] Fragment shader compiled successfully\n", fragmentShader);

    program = glCreateProgram();
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
	
    glLinkProgram(program);

    // NOTE: All uniform variables are initialized to 0 when a program links
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        debug("[SHDR ID %i] Failed to link shader program...\n", program);
        internal_gpu_debug(program);
        
        glDeleteProgram(program);
        
        program = 0;
    }
    else debug("[SHDR ID %i] Shader program loaded successfully\n", program);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void shader_free(shader* sh)
{
    glUseProgram(0);

   	glDisableVertexAttribArray(sh->vertex_location);
    glDisableVertexAttribArray(sh->texture_location);

    glDeleteProgram(sh->id);
    
    debug("Shader %i unloaded\n", sh->id);
    
    free(sh);
    sh = NULL;
}

shader* shader_alloc(const char* filename)
{
	shader* sh = calloc(1, sizeof(shader));

    char vertex_file[64];
    char fragment_file[64];

    strcpy(vertex_file, filename);
    strcpy(fragment_file, filename);

    strcat(vertex_file, ".vs");
    strcat(fragment_file, ".fs");

    char* vertex_str = load_text(vertex_file);
    char* fragment_str = load_text(fragment_file);

    sh->id = internal_load_shader_program(vertex_str, fragment_str);
    
    if (sh->id != 0)
    {
        debug("[SHDR ID %i] %s shader loaded successfully\n", sh->id, filename);
    }
    else
    {
        debug("[SHDR ID %i] %s shader could not be loaded\n", sh->id, filename);
    }
    
    if (sh->id != 0)
    {
        sh->vertex_location = glGetAttribLocation(sh->id, "vertex_location");
		sh->texture_location = glGetAttribLocation(sh->id, "texture_location");
		sh->normal_location = glGetAttribLocation(sh->id, "normal_location");		
        sh->tint_location = glGetUniformLocation(sh->id, "tint_location");
        sh->time_location = glGetUniformLocation(sh->id, "time_location");
        sh->resolution_location = glGetUniformLocation(sh->id, "resolution_location");

		// position and intensity
        sh->light_location = glGetUniformLocation(sh->id, "light_location");
        sh->pov_location = glGetUniformLocation(sh->id, "pov_location");
        sh->reference_location = glGetUniformLocation(sh->id, "reference_location");

        // Matrices uniforms        
        sh->model_matrix = glGetUniformLocation(sh->id, "modelMatrix");
        sh->normal_matrix = glGetUniformLocation(sh->id, "normalMatrix");
        sh->view_matrix = glGetUniformLocation(sh->id, "viewMatrix");
        sh->mvp_matrix = glGetUniformLocation(sh->id, "mvpMatrix");

        debug("[SHDR ID %i] %s shader locations set\n", sh->id, filename);
    }

    return sh;
}
