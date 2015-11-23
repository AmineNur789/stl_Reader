
#include "stdafx.h"
#define GLEW_STATIC
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h> 
#include <Gl/glut.h>
#include <iostream>
using namespace std;

#define BUFSIZE 512
GLfloat scale = 1.0;
static float M_PI = 3.1415926;
static float c = M_PI / 180.0f; //���ȺͽǶ�ת������
static int du = 80, oldmy = -1, oldmx = -1; //du���ӵ���y��ĽǶ�,opengl��Ĭ��y�����Ϸ���
static float r = 5.0f, h = 2.0f; //r���ӵ���y��İ뾶,h���ӵ�߶ȼ���y���ϵ�����
GLfloat normal[100][3];
GLfloat vertex[100][3];
int triangle_num = 0, normal_num = 0;

void readStl()
{
	//��stl�ļ�����ʾ
	FILE *fp = fopen("next_prueba.stl", "r");
	if (fp != nullptr)
	{
		char str[100], test[100];

		//GLfloat color_v = 0.1;
		while (1)
		{
			fscanf(fp, "%s", &str);
			if (!strcmp(str, "facet"))
			{
				fscanf(fp, "%s", &test);
				fscanf(fp, "%f %f %f\n", &normal[normal_num][0], &normal[normal_num][1], &normal[normal_num][2]);
				normal_num++;
				//glNormal3fv(normal[triangle_num]);
				continue;
			}
			else if (!strcmp(str, "outer"))
			{
				fscanf(fp, "%s", &test);
				for (int i = 0; i < 3; i++)
				{
					fscanf(fp, "%s", &test);
					fscanf(fp, "%f %f %f\n", &vertex[triangle_num][0], &vertex[triangle_num][1], &vertex[triangle_num][2]);
					//cout << vertex[0] << " " << vertex[1] << " " << vertex[2] << endl;
					triangle_num++;
				}
				continue;
			}
			else if (!strcmp(str, "endsolid"))
			{
				break;
			}
			else
			{
				continue;
			}
		}
	}
	fclose(fp);
}
void drawStl(GLenum mode)
{
	int i = 0, j = 0;
	for (j = 0, i = 0; j < normal_num; j++)
	{
		glNormal3fv(normal[j]);
		if (mode == GL_SELECT)
			glLoadName(j+1);
		glBegin(GL_TRIANGLES);
		glColor3d(0.0f, 0.0f, 1.0f);
		glVertex3f(vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glVertex3f(vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glVertex3f(vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glEnd();
	}
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(r*cos(c*du), h, r*sin(c*du), 0, 0, 0, 0, 1, 0); //���ӵ㿴Զ��,y�᷽��(0,1,0)���Ϸ���
	drawStl(GL_RENDER);

	glFlush();
	glutSwapBuffers();
}
void processHits(GLint hits, GLuint buffer[])
{
	GLint i;
	GLuint j;
	GLuint names, *ptr;
	printf("hit=%d\n", hits);
	ptr = (GLuint *)buffer;
	for (i = 0; i<hits; i++)
	{
		names = *ptr;
		//printf("number of names for this hit =%d\n", names);

		ptr++;
		//printf("z1 is %g;\n", (float)*ptr / 0x7fffffff);//selectBuffer�����б���������ջ�����ֵĸ�����ѡ�񵽵��������С������ֵ
		ptr++;
		//printf("z2 is %g;\n", (float)*ptr / 0x7fffffff);//������z1������ʾ��������С�����ֵ��z2������ʾ�������������ֵ��
		//���ֵ��0~1��ֵ�������������Ϊ0��Զ��Ϊ1������selectbuffer�����ε����飬��˱����ʱ�������0x7fffffff�������ʾ��ʱ��ҲҪ���Ը�ֵ

		ptr++;
		printf("names are ");

		for (j = 0; j<names; j++)
		{
			printf("%d", *ptr);
			ptr++;
		}
		printf("\n");
	}
}
void Mouse(int button, int state, int x, int y) //���������
{
	//printf("button: %d state: %d\n", button, state);
		if (button == GLUT_LEFT_BUTTON)
		{
			oldmx = x, oldmy = y;
			GLuint selectBuff[BUFSIZE];
			GLint hits, viewport[4];
			if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
				return;

			glGetIntegerv(GL_VIEWPORT, viewport); //���viewport
			glSelectBuffer(BUFSIZE, selectBuff); //����OpenGL��ʼ��selectbuffer 
			(void)glRenderMode(GL_SELECT); //����ѡ��ģʽ
			glInitNames();//��ʼ������ջ
			glPushName(0);//������ջ�з���һ����ʼ������

			glMatrixMode(GL_PROJECTION);//����ͶӰ�׶�׼��ʰȡ
			glPushMatrix();//������ǰ��ͶӰ����
			glLoadIdentity();//���뵥λ����
			gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 1.0, 1.0, viewport);
			//Ӱ�컭�������
			//ͶӰ�任Ϊ��ͶӰ
			gluPerspective(50.0f, 1.0f, 0.1f, 1000.0f);

			drawStl(GL_SELECT);
			//display();
			glPopMatrix();
			glFlush();
			glutSwapBuffers();
			hits = glRenderMode(GL_RENDER);
			// ��ѡ��ģʽ��������ģʽ,�ú�������ѡ�񵽶���ĸ���
			processHits(hits, selectBuff);
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		{
			oldmx = x, oldmy = y;
			scale = (scale == 1.0f ? 0.5f : 1.0f);
			printf("scale: %f\n", scale);
		}
}
void onMouseMove(int x, int y) //��������϶�
{
	du += x - oldmx; //����ڴ���x�᷽���ϵ������ӵ��ӵ���y��ĽǶ��ϣ�����������ת��
	h += 0.03f*(y - oldmy); //����ڴ���y�᷽���ϵĸı�ӵ��ӵ��y�����ϣ�������ת��
	if (h>10.0f) h = 2.0f; //�ӵ�y������һЩ���ƣ�����ʹ�ӵ�̫���
	else if (h<-10.0f) h = -2.0f;
	oldmx = x, oldmy = y; //�Ѵ�ʱ�����������Ϊ��ֵ��Ϊ��һ�μ���������׼��
	
}
void init()
{
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)w);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (float)w / h, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
int main(int argc, char *argv[])
{
	readStl();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("OpenGL_task1_picking");
	init();

	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);//��ʾ�ص�
	glutIdleFunc(display);
	glutMainLoop();

	return 0;
}

