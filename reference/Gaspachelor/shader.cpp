//#include <cstdio>
#include <fstream>
#include "shader.h"
#include "error.h"

std::string readVertex( const std::string &in );
std::string readFragment( const std::string &in );

/**
    textFileRead loads in a standard text file from a given filename and
    then returns it as a std::string.
*/
static std::string textFileRead(const char *fileName) {
    std::string fileString = std::string(); // A std::string for storing the file contents
    std::string line = std::string(); // A std::string for holding the current line

    std::ifstream file(fileName, std::ios::in); // Open an input stream with the selected file
    if (file.is_open()) { // If the file opened successfully
        while (!file.eof()) { // While we are not at the end of the file
            getline(file, line); // Get the current line
            fileString.append(line); // Append the line to our file std::string
            fileString.append("\n"); // Appand a new line character
        }
        file.close(); // Close the file
    }

    return fileString; // Return our std::string
}

/**
    Given a shader and the filename associated with it, validateShader will
    then get information from OpenGl on whether or not the shader was compiled successfully
    and if it wasn't, it will output the file with the problem, as well as the problem.
*/
static void validateShader(GLuint shader) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    //memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader
    if (length > 0) // If we have any information to display
        std::cout << "Shader " << shader << " compile error: " << buffer << std::endl; // Output the information
}

/**
    Given a shader program, validateProgram will request from OpenGL, any information
    related to the validation or linking of the program with it's attached shaders. It will
    then output any issues that have occurred.
*/
static void validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    //memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
    if (length > 0) // If we have any information to display
        std::cout << "Program " << program << " link error(s): "<< std::endl << buffer << std::endl; // Output the information
    
    glValidateProgram(program); // Get OpenGL to try validating the program
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
    if (status == GL_FALSE) // If there was a problem validating
        std::cout << "Error validating shader " << program << std::endl; // Output which program had the error
}

Shader::Shader()
{
    compiled=false;
}

Shader::Shader( const std::string &filename )
{
    compiled = false;

    init(filename);

    GLint result;
    result = glGetUniformLocation(program,"projMatrix");
    if(result>-1)
        projMatrixLoc = (GLuint)result;
    else
        printError("Shader Construct","Could not get uniform projMatrix location");

    result = glGetUniformLocation(program,"viewMatrix");
    if(result>-1)
        viewMatrixLoc = (GLuint)result;
    else
        printError("Shader Construct","Could not get uniform viewMatrix location");

    std::cout<<"projMatrix "<<projMatrixLoc<<std::endl;
    std::cout<<"viewMatrix "<<viewMatrixLoc<<std::endl;
}

Shader::~Shader() {
    glDetachShader(program, fragmentShader); // Detach the fragment shader
    glDetachShader(program, vertexShader); // Detach the vertex shader
    
    glDeleteShader(fragmentShader); // Delete the fragment shader
    glDeleteShader(vertexShader); // Delete the vertex shader
    glDeleteProgram(program); // Delete the shader program
}

void Shader::init(const std::string &filename) {
    if (compiled) // If we have already initialized the shader
        return;

    compiled = true; // Mark that we have initialized the shader
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader

    std::string text = textFileRead(filename.c_str());
    std::string vsText = readVertex(text); // Read in the vertex shader
    std::string fsText = readFragment(text); // Read in the fragment shader
    
    const char *vertexText = vsText.c_str();
    const char *fragmentText = fsText.c_str();

    if (vertexText == NULL || fragmentText == NULL) { // If either the vertex or fragment shader wouldn't load
        std::cout << "Either vertex shader or fragment shader file not found." << std::endl; // Output the error
        return;
    }
    
    glShaderSource(vertexShader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
    glCompileShader(vertexShader); // Compile the vertex shader
    validateShader(vertexShader); // Validate the vertex shader
    
    glShaderSource(fragmentShader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
    glCompileShader(fragmentShader); // Compile the fragment shader
    validateShader(fragmentShader); // Validate the fragment shader
    
    program = glCreateProgram(); // Create a GLSL program
    glAttachShader(program, vertexShader); // Attach a vertex shader to the program
    glAttachShader(program, fragmentShader); // Attach the fragment shader to the program

    //glBindAttribLocation(program, 0, "in_position"); // Bind a constant attribute location for positions of vertices
    //glBindAttribLocation(program, 1, "in_normal"); // Bind another constant attribute location, this time for color
    //glBindAttribLocation(program, 2, "in_tangent");
    //glBindAttribLocation(program, 3, "in_color");
    //glBindAttribLocation(program, 4, "in_texCoord");

    //glBindFragDataLocation(programId, 0, "myVec41");
    //glBindFragDataLocation(programId, 1, "myVec42");

    glLinkProgram(program); // Link the vertex and fragment shaders in the program
    validateProgram(program); // Validate the shader program

    GLint isLinked;

    glGetProgramiv(program, GL_LINK_STATUS, (GLint *)&isLinked);
    if(isLinked==GL_FALSE)
    {
        printError("Shader init","Failed to link shader");
 
        GLint maxLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        if(maxLength>0)
        {
            char *pLinkInfoLog = new char[maxLength];
            glGetProgramInfoLog(program, maxLength, &maxLength, pLinkInfoLog);
            std::cout << pLinkInfoLog << std::endl;
            delete [] pLinkInfoLog;
        }
 
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        vertexShader=0;
        glDeleteShader(fragmentShader);
        fragmentShader=0;
        glDeleteProgram(program);
        program=0;
 
        return;
    }

    std::cout<<"Shader "<<filename<<" compiled"<<std::endl;
}


GLint Shader::getAttributeLocation(const std::string &att)
{
    return glGetAttribLocation(program, att.c_str());
}


GLint Shader::getUniformLocation(const std::string &uni)
{
    return glGetUniformLocation(program, uni.c_str());

}

void Shader::bind()
{
	if(compiled)
		glUseProgram(program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

std::string readVertex( const std::string &in )
{

    std::string out;
    std::size_t spos;
    int pos;
    int endPos=0;
    
    spos = in.find("#VERTEX#");
    if(spos != std::string::npos)
    {
        pos = int(spos) + 8;
        
        spos = in.find("#END VERTEX#");
        if(spos != std::string::npos)
        	endPos = int(spos);
        
        out = in.substr(pos, endPos - pos);
    }
    
    return out;
}

std::string readFragment( const std::string &in )
{

    std::string out;
    std::size_t spos;
    int pos;
    int endPos=0;
    
    spos = in.find("#FRAGMENT#");
    if(spos != std::string::npos)
    {
        pos = int(spos) + 10;
        spos = in.find("#END FRAGMENT#");
        if(spos != std::string::npos)
        	endPos = int(spos);
        out = in.substr(pos, endPos - pos);
    }
    
    return out;
}