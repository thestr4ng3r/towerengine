#ifndef _CTEXTURE_H
#define _CTEXTURE_H

class CTexture
{
public:
	GLuint diffuse_image;
	GLuint specular_image;
	GLuint normal_image;
	GLuint aeb_image;
	GLuint height_image;
	struct diffuse
	{
		char *filename;  	/* Default: NULL */
		CVector color;        	/* Default: 1.0, 1.0, 1.0 */
		CVector ambient_color;	/* Default: 0.3, 0.3, 0.3 */
	} diffuse;
	struct specular
	{
		char *filename;  	/* Default: NULL */
		CVector color;         	/* Default: 1.0, 1.0, 1.0 */ 
	} specular;
	struct normal
	{
		char *filename;  	/* Default: NULL */
	} normal;
	struct aeb
	{
		char *filename;
		float bump_factor;
		float exponent_factor;
	} aeb;
	struct height
	{
		char *filename;  	/* Default: NULL */
		CVector factor;
	} height;

	bool transparent;

	CTexture(void);
	~CTexture(void);

	void Load(void);
	void PutToGL(void);

	static GLuint default_white;
	static GLuint default_normal;

	static void LoadDefaultTextures(void);
	static void PutDefaultToGL(void);

	void Copy(CTexture *t);
};

#endif


