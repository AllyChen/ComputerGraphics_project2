#include "main.h"

#include "SOIL\SOIL.h"

vec3 camera = vec3(0,0,20);
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//以OpenGL version4.3版本為基準
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//是否向下相容,GLUT_FORWARD_COMPATIBLE不支援(?
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Ally Robot");

	glewExperimental = GL_TRUE; //置於glewInit()之前
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	int ActionMenu,ModeMenu,ShaderMenu;
	ActionMenu = glutCreateMenu(ActionMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("idle",0);
	glutAddMenuEntry("walk",1);
	glutAddMenuEntry("guard", 2);
	glutAddMenuEntry("think", 3);
	glutAddMenuEntry("bear", 4);
	glutAddMenuEntry("hug", 5);
	glutAddMenuEntry("hello", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ModeMenu = glutCreateMenu(ModeMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("Line",0);
	glutAddMenuEntry("Fill",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	ShaderMenu = glutCreateMenu(ShaderMenuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddMenuEntry("close", 0);
	glutAddMenuEntry("old", 1);
	glutAddMenuEntry("pixel", 2);
	glutAddMenuEntry("rotate", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	glutCreateMenu(menuEvents);//建立右鍵菜單
	//加入右鍵物件
	glutAddSubMenu("action",ActionMenu);
	glutAddSubMenu("mode",ModeMenu);
	glutAddSubMenu("shader", ShaderMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//與右鍵關聯

	glutMouseFunc(Mouse);
	glutTimerFunc (100, Timer, 0); 
	glutMainLoop();
	return 0;
}
void ChangeSize(int w,int h){
	if(h == 0) h = 1;
	glViewport(0,0,w,h);
	//Projection = perspective(80.0f,(float)w/h,0.1f,100.0f);
	Projection = perspective(70.0f, (float)w / h, 0.1f, 100.0f);
}
void Mouse(int button,int state,int x,int y){
	if(button == 2) isFrame = false;
}
void idle(int dummy){
	isFrame = true;
	int out = 0;
	if(action == WALK){
		updateObj(dummy);
		out = dummy+1;
		if(out > 12) out = 1;
	}
	else if(action == IDLE){
		resetObj(dummy);
		out = 0;
	}
	glutPostRedisplay();
	
	glutTimerFunc (150, idle, out); 
}
void resetObj(int f){
	for(int i = 0 ; i < PARTSNUM;i++){
		angles[i] = 0.0f;
	}	
}
void updateObj(int frame){
	switch(frame){
	case 0:
		//左手
		angles[2] = -45;
		//右手

		//腿
		angles[13] = 45;	
		
		break;
	case 1:
	case 2:
	case 3:
		angles[1] +=10;
		angles[12] -=15;
		position += 0.1;
		break;
	case 4:
	case 5:
	case 6:
		angles[1] -=10;
		angles[12] +=15;
		angles[13] -= 15;
		position -= 0.1;
		break;
	case 7:
	case 8:
	case 9:
		angles[1] -=10;
		angles[12] +=15;
		angles[13] = 0;
		position += 0.1;
		break;
	case 10:
	case 11:
	case 12:
		angles[1] +=10;
		angles[12] -=15;
		angles[13] += 15;
		position -= 0.1;
		break;
	}
}


 GLuint M_KaID;
 GLuint M_KdID;
 GLuint M_KsID;

void init(){
	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//初始化角度陣列
		angles[i] = 0.0;

	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "DSPhong_Material.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "DSPhong_Material.fp" },//fragment shader
		{ GL_NONE, NULL }};
	program = LoadShaders(shaders);//讀取shader

	glUseProgram(program);//uniform參數數值前必須先use shader
	
	MatricesIdx = glGetUniformBlockIndex(program,"MatVP");
	ModelID =  glGetUniformLocation(program,"Model");
    M_KaID = glGetUniformLocation(program,"Material.Ka");
	M_KdID = glGetUniformLocation(program,"Material.Kd");
	M_KsID = glGetUniformLocation(program,"Material.Ks");
	t_ID = glGetUniformLocation(program, "rotateShader_t");
	//or
	M_KdID = M_KaID+1;
	M_KsID = M_KaID+2;

	Projection = glm::perspective(80.0f,4.0f/3.0f,0.1f,100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	
	// Camera matrix
	View       = glm::lookAt(
		glm::vec3(0,10,25) , // Camera is at (0,10,25), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,1,0 to look upside-down)
		);

	Obj2Buffer();

	//UBO
	glGenBuffers(1,&UBO);
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(mat4)*2,NULL,GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);  
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER,0,UBO,0,UBOsize);
	glUniformBlockBinding(program, MatricesIdx,0);

	glClearColor(1.0,1.0,1.0,1);//black screen
}

#define DOR(angle) (angle*3.1415/180);
void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);
	glUseProgram(program);//uniform參數數值前必須先use shader
	float eyey = DOR(eyeAngley);
	View       = lookAt(
		               vec3(eyedistance*sin(eyey),2,eyedistance*cos(eyey)) , // Camera is at (0,0,20), in World Space
		               vec3(0,0,0), // and looks at the origin
		               vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		                );
	updateModels();
	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(mat4),&View);
	glBufferSubData(GL_UNIFORM_BUFFER,sizeof(mat4),sizeof(mat4),&Projection);
	glBindBuffer(GL_UNIFORM_BUFFER,0);

	glUniform1f(t_ID, rotateShader_t);

	GLuint offset[3] = {0,0,0};//offset for vertices , uvs , normals
	for(int i = 0;i < PARTSNUM ;i++){
		glUniformMatrix4fv(ModelID,1,GL_FALSE,&Models[i][0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
							  3,				//vec3
							  GL_FLOAT,			//type
							  GL_FALSE,			//not normalized
							  0,				//strip
							  (void*)offset[0]);//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] +=  vertices_size[i]*sizeof(vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glVertexAttribPointer(1, 
							  2, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[1]);
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] +=  uvs_size[i]*sizeof(vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glVertexAttribPointer(2,
							  3, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[2]);
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] +=  normals_size[i]*sizeof(vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		string mtlname;//material name
		vec3 Ks = vec3(1,1,1);//because .mtl excluding specular , so give it here.
		for(int j = 0;j <mtls[i].size() ;j++){//
			mtlname = mtls[i][j];	
			//find the material diffuse color in map:KDs by material name.
			glUniform3fv(M_KdID,1,&KDs[mtlname][0]);
			glUniform3fv(M_KsID,1,&Ks[0]);
			//          (primitive   , glVertexID base , vertex count    )
			glDrawArrays(GL_TRIANGLES, vertexIDoffset  , faces[i][j+1]*3);
			//we draw triangles by giving the glVertexID base and vertex count is face count*3
			vertexIDoffset += faces[i][j+1]*3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	
		
	}//end for loop for updating and drawing model
	glFlush();//強制執行上次的OpenGL commands
	glutSwapBuffers();//調換前台和後台buffer ,當後臺buffer畫完和前台buffer交換使我們看見它
}

void Obj2Buffer(){
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;
	//loadMTL("Obj/gundam.mtl",Kds,Kas,Kss,Materials,texture);
	loadMTL("ModelLess/PandaMTL.mtl", Kds, Kas, Kss, Materials, texture);
	//printf("%d\n",texture);
	for(int i = 0;i<Materials.size();i++){
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}	

{
	/*load2Buffer("Obj/body.obj",0);

	load2Buffer("Obj/ulefthand.obj",1);
	load2Buffer("Obj/dlefthand.obj",2);
	load2Buffer("Obj/lefthand.obj",3);
	load2Buffer("Obj/lshouder.obj",4);
	
	load2Buffer("Obj/head.obj",5);*/

	/*load2Buffer("Model/ModelLess/body.obj", 0);
	
	load2Buffer("Model/ModelLess/Left_hand.obj", 6);
	load2Buffer("Model/ModelLess/Left_shoulder.obj", 9);
	
	load2Buffer("Model/ModelLess/head.obj", 5);
	
	load2Buffer("Model/ModelLess/Right_hand.obj", 1);
	load2Buffer("Model/ModelLess/Right_shoulder.obj", 4);
	
	load2Buffer("Model/ModelLess/clothes.obj", 11);
	
	load2Buffer("Model/ModelLess/Left_upLeg.obj", 12);
	load2Buffer("Model/ModelLess/Left_downLeg.obj", 13);
	
	load2Buffer("Model/ModelLess/Right_upLeg.obj", 15);
	load2Buffer("Model/ModelLess/Right_downLeg.obj", 16);

	load2Buffer("Model/ModelLess/pants.obj", 10);

	load2Buffer("Model/ModelLess/Shield.obj", 3);*/


	//load2Buffer("Obj/urighthand.obj",6);
	//load2Buffer("Obj/drighthand.obj",7);
	//load2Buffer("Obj/righthand.obj",8);
	//load2Buffer("Obj/rshouder.obj",9);

	//load2Buffer("Obj/dbody.obj",11);
	//load2Buffer("Obj/back2.obj",10);

	//load2Buffer("Obj/uleftleg.obj",12);
	//load2Buffer("Obj/dleftleg.obj",13);
	//load2Buffer("Obj/leftfoot.obj",14);

	//load2Buffer("Obj/urightleg.obj",15);	
	//load2Buffer("Obj/drightleg.obj",16);	
	//load2Buffer("Obj/rightfoot.obj",17);
}

	textureID = LoadTexture("Texture/texture-9.png");

	load2Buffer("ModelLess/body-1.obj", 0);

	load2Buffer("ModelLess/head-1.obj", 1);

	load2Buffer("ModelLess/Left_shoulder-1.obj", 2);
	load2Buffer("ModelLess/Left_upArm-1.obj", 3);
	load2Buffer("ModelLess/Left_downArm-1.obj", 13);

	load2Buffer("ModelLess/Right_shoulder-1.obj", 4);
	load2Buffer("ModelLess/Right_upArm-1.obj", 5);
	load2Buffer("ModelLess/Right_downArm-1.obj", 14);

	load2Buffer("ModelLess/clothes-1.obj", 6);

	load2Buffer("ModelLess/Left_upLeg-1.obj", 8);
	load2Buffer("ModelLess/Left_downLeg-1.obj", 9);

	load2Buffer("ModelLess/Right_upLeg-1.obj", 10);
	load2Buffer("ModelLess/Right_downLeg-1.obj", 11);

	load2Buffer("ModelLess/pants-1.obj", 7);

	load2Buffer("ModelLess/Shield.obj", 12);

	
	GLuint totalSize[3] = {0,0,0};
	GLuint offset[3] = {0,0,0};
	for(int i = 0;i < PARTSNUM ;i++){
		totalSize[0] += vertices_size[i]*sizeof(vec3);
		totalSize[1] += uvs_size[i] * sizeof(vec2);
		totalSize[2] += normals_size[i] * sizeof(vec3);
	}
	//generate vbo
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&uVBO);
	glGenBuffers(1,&nVBO);
	//bind vbo ,第一次bind也同等於 create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[2],NULL,GL_STATIC_DRAW);
	
	
	for(int i = 0;i < PARTSNUM ;i++){
		glBindBuffer(GL_COPY_WRITE_BUFFER,VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[0],vertices_size[i]*sizeof(vec3));
		offset[0] += vertices_size[i]*sizeof(vec3);
		glInvalidateBufferData(VBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[1],uvs_size[i]*sizeof(vec2));
		offset[1] += uvs_size[i]*sizeof(vec2);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[2],normals_size[i]*sizeof(vec3));
		offset[2] += normals_size[i] * sizeof(vec3);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	}
	glBindBuffer(GL_COPY_WRITE_BUFFER,0);


}

void updateModels(){
	mat4 Rotatation[PARTSNUM];
	mat4 Translation[PARTSNUM];
	for(int i = 0 ; i < PARTSNUM;i++){
		Models[i] = mat4(1.0f);
		Rotatation[i] = mat4(1.0f);
		Translation[i] = mat4(1.0f); 
	}
	float r,pitch,yaw,roll;
	float alpha, beta ,gamma;

	// Body
	Translation[A_BODY] = translate(0, 0 + PandaTranslate_Y[A_BODY], 0 + PandaTranslate_Z[A_BODY]);
	Models[A_BODY] = Translation[A_BODY];

	// Head
	Rotatation[A_HEAD] = rotate(PandaAngles[A_HEAD], PandaAnglesDirection[A_HEAD][0], PandaAnglesDirection[A_HEAD][1], PandaAnglesDirection[A_HEAD][2]);
	Translation[A_HEAD] = translate(0.017, 2.186 + PandaTranslate_Y[A_HEAD], 0) + PandaTranslate_Z[A_HEAD];
	Models[A_HEAD] = Models[A_BODY] * Translation[A_HEAD] * Rotatation[A_HEAD];

	// Left_shoulder
	Rotatation[A_LEFT_SHOULDER] = rotate(PandaAngles[A_LEFT_SHOULDER], 1, 0, 0);
	Translation[A_LEFT_SHOULDER] = translate(-1.266, 1.859 + PandaTranslate_Y[A_LEFT_SHOULDER], -0.297 + PandaTranslate_Z[A_LEFT_SHOULDER]);
	Models[A_LEFT_SHOULDER] = Models[A_BODY] * Translation[A_LEFT_SHOULDER] * Rotatation[A_LEFT_SHOULDER];
	// Left_hand
	Translation[A_LEFT_HAND] = translate(-2.443 + 1.266, 2.709 - 1.859 + PandaTranslate_Y[A_LEFT_HAND], -0.339 + 0.297 + PandaTranslate_Z[A_LEFT_HAND]);
	Models[A_LEFT_HAND] = Models[A_LEFT_SHOULDER] * Translation[A_LEFT_HAND];
	// Left_downArm
	Rotatation[A_LEFT_DOWNARM] = rotate(PandaAngles[A_LEFT_DOWNARM], 0, 0, 1);
	Translation[A_LEFT_DOWNARM] = translate(-3.303 - (-2.443), 0 - (2.709) + PandaTranslate_Y[A_LEFT_DOWNARM], -0.484 - (-0.339) + PandaTranslate_Z[A_LEFT_DOWNARM]);
	Models[A_LEFT_DOWNARM] = Models[A_LEFT_HAND] * Translation[A_LEFT_DOWNARM] * Rotatation[A_LEFT_DOWNARM];

	// Right_shoulder
	Rotatation[A_RIGHT_SHOULDER] = rotate(PandaAngles[A_RIGHT_SHOULDER], 1, 0, 0);
	Translation[A_RIGHT_SHOULDER] = translate(1.275, 1.859 + PandaTranslate_Y[A_RIGHT_SHOULDER], -0.297 + PandaTranslate_Z[A_RIGHT_SHOULDER]);
	Models[A_RIGHT_SHOULDER] = Models[A_BODY] * Translation[A_RIGHT_SHOULDER] * Rotatation[A_RIGHT_SHOULDER];
	// Right_hand
	Rotatation[A_RIGHT_HAND] = rotate(PandaAngles[A_RIGHT_HAND], 1, 0, 0);
	Translation[A_RIGHT_HAND] = translate(2.443 - 1.275, 2.709 - 1.859 + PandaTranslate_Y[A_RIGHT_HAND], -0.339 + 0.297 + PandaTranslate_Z[A_RIGHT_HAND]);
	Models[A_RIGHT_HAND] = Models[A_RIGHT_SHOULDER] * Translation[A_RIGHT_HAND];
	// Right_downArm
	Rotatation[A_RIGHT_DOWNARM] = rotate(PandaAngles[A_RIGHT_DOWNARM], 0, 0, 1);
	Translation[A_RIGHT_DOWNARM] = translate(3.303 - (2.443), 0 - (2.709) + PandaTranslate_Y[A_RIGHT_DOWNARM], -0.484 - (-0.339) + PandaTranslate_Z[A_RIGHT_DOWNARM]);
	Models[A_RIGHT_DOWNARM] = Models[A_RIGHT_HAND] * Translation[A_RIGHT_DOWNARM] * Rotatation[A_RIGHT_DOWNARM];

	// Shield
	Rotatation[A_SHIELD] = rotate(PandaAngles[A_SHIELD], 1, 0, 0);
	Translation[A_SHIELD] = translate(4.717 - 3.303, -0.409 - 0 + PandaTranslate_Y[A_SHIELD], 0.395 - (-0.484) + PandaTranslate_Z[A_SHIELD]);
	Models[A_SHIELD] = Models[A_RIGHT_DOWNARM] * Translation[A_SHIELD] * Rotatation[A_SHIELD];

	// clothes
	Translation[A_CLOTHES] = translate(0.018, 1.061 + PandaTranslate_Y[A_CLOTHES], 0.1 + PandaTranslate_Z[A_CLOTHES]);
	Models[A_CLOTHES] = Models[A_BODY] * Translation[A_CLOTHES];
	
	// Left_upLeg
	Rotatation[A_LEFT_UP_LEG] = rotate(PandaAngles[A_LEFT_UP_LEG], PandaAnglesDirection[A_LEFT_UP_LEG][0], PandaAnglesDirection[A_LEFT_UP_LEG][1], PandaAnglesDirection[A_LEFT_UP_LEG][2]);
	Translation[A_LEFT_UP_LEG] = translate(-1.359, -0.482 + PandaTranslate_Y[A_LEFT_UP_LEG], 0 + PandaTranslate_Z[A_LEFT_UP_LEG]);
	Models[A_LEFT_UP_LEG] = Models[A_BODY] * Translation[A_LEFT_UP_LEG] * Rotatation[A_LEFT_UP_LEG];
	// Left_downLeg
	Rotatation[A_LEFT_DOWN_LEG] = rotate(PandaAngles[A_LEFT_DOWN_LEG], 1, 0, 0);
	Translation[A_LEFT_DOWN_LEG] = translate(-1.354 + 1.359, -1.917 + 0.482 + PandaTranslate_Y[A_LEFT_DOWN_LEG], 0 + PandaTranslate_Z[A_LEFT_DOWN_LEG]);
	Models[A_LEFT_DOWN_LEG] = Models[A_LEFT_UP_LEG] * Translation[A_LEFT_DOWN_LEG] * Rotatation[A_LEFT_DOWN_LEG];

	// Right_upLeg
	Rotatation[A_RIGHT_UP_LEG] = rotate(PandaAngles[A_RIGHT_UP_LEG], PandaAnglesDirection[A_RIGHT_UP_LEG][0], PandaAnglesDirection[A_RIGHT_UP_LEG][1], PandaAnglesDirection[A_RIGHT_UP_LEG][2]);
	Translation[A_RIGHT_UP_LEG] = translate(1.369, -0.482 + PandaTranslate_Y[A_RIGHT_UP_LEG], 0 + PandaTranslate_Z[A_RIGHT_UP_LEG]);
	Models[A_RIGHT_UP_LEG] = Models[A_BODY] * Translation[A_RIGHT_UP_LEG] * Rotatation[A_RIGHT_UP_LEG];
	// Right_downLeg
	Rotatation[A_RIGHT_DOWN_LEG] = rotate(PandaAngles[A_RIGHT_DOWN_LEG], 1, 0, 0);
	Translation[A_RIGHT_DOWN_LEG] = translate(1.364 - 1.369, -1.917 + 0.482 + PandaTranslate_Y[A_RIGHT_DOWN_LEG], 0 + PandaTranslate_Z[A_RIGHT_DOWN_LEG]);
	Models[A_RIGHT_DOWN_LEG] = Models[A_RIGHT_UP_LEG] * Translation[A_RIGHT_DOWN_LEG] * Rotatation[A_RIGHT_DOWN_LEG];

	// pants
	Translation[A_PANTS] = translate(-0.001, -0.112-1.2 + PandaTranslate_Y[A_PANTS], 0 + PandaTranslate_Z[A_PANTS]);
	Models[A_PANTS] = Models[A_BODY] * Translation[A_PANTS];
	
	{
	////Body
	//beta = angle;
	//Rotatation[0] = rotate(beta,0,1,0);
	//Translation[0] = translate(0,2.9+position,0);
	//Models[0] = Translation[0]*Rotatation[0];
	////左手=======================================================
	//左上手臂
	//yaw = DOR(beta);r = 3.7;
	//alpha = angles[1];
	//gamma = 10;
	//Rotatation[1] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	//Translation[1] = translate(3.7,1,-0.5);

	//Models[1] = Models[5]*Translation[1]*Rotatation[1];
	
	////左肩膀
	//Rotatation[4] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	//Translation[4] =translate(3.7,1,-0.5);//位移到左上手臂處
	//Models[4] =Models[0]*Translation[1]*Rotatation[1];
	//
	////左下手臂
	//pitch = DOR(alpha);r = 3;
	//roll = DOR(gamma);
	//static int i=0;
	//i+=5;
	//alpha = angles[2]-20;
	////上手臂+下手臂向前旋轉*向右旋轉
	//Rotatation[2] = rotate(alpha,1,0,0);
	////延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin)
	////延z軸位移以上手臂為半徑角度:translate(r*sin,-rcos,0)
	//Translation[2] = translate(0,-3,0);

	//Models[2] = Models[1]*Translation[2]*Rotatation[2];
	//

	//pitch = DOR(alpha);
	////b = DOR(angles[2]);
	//roll = DOR(gamma);
	////手掌角度與下手臂相同
	////Rotatation[3] = Rotatation[2];
	////延x軸位移以上手臂為半徑的圓周長:translate(0,r*cos,r*sin) ,角度為上手臂+下手臂
	//Translation[3] = translate(0,-4.8,0);
	//Models[3] = Models[2]*Translation[3]*Rotatation[3];
	////============================================================
	////頭==========================================================
	//Translation[5] = translate(0,3.9,-0.5);
	//Models[5] = Models[0]*Translation[5]*Rotatation[5];
	////============================================================
	////右手=========================================================
	//gamma = -10;alpha = angles[6] = -angles[1];
	//Rotatation[6] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	//Translation[6] = translate(-3.9,1.7,-0.2);
	//Models[6] = Models[0]*Translation[6]*Rotatation[6];

	//Rotatation[9] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	//Translation[9] = translate(-3.9,1.1,-0.2);
	//Models[9] = Models[0]*Translation[9]*Rotatation[9];

	//angles[7] = angles[2];
	//pitch = DOR(alpha);r = -3;
	//roll = DOR(gamma);
	//alpha = angles[7]-20;
	//Rotatation[7] = rotate(alpha,1,0,0);
	//Translation[7] = translate(0,-3,0);
	//Models[7] = Models[6]*Translation[7]*Rotatation[7];

	//pitch = DOR(alpha);
	////b = DOR(angles[7]);
	//roll = DOR(gamma);
	//Translation[8] =translate(0,-6,0);
	//Models[8] = Models[7]*Translation[8]*Rotatation[8];
	////=============================================================
	////back&DBody===================================================
	//Translation[10] =translate(0,2,-4.5);
	//Models[10] = Models[0]*Translation[10]*Rotatation[10];

	//Translation[11] =translate(0,-5.3,0);
	//Models[11] = Models[0]*Translation[11]*Rotatation[11];
	////=============================================================
	////左腳
	//alpha = angles[12];gamma = 10;
	//Rotatation[12] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	//Translation[12] =translate(1.8,-4.5,0);
	//Models[12] = Translation[12]*Rotatation[12]*Models[12];

	//pitch = DOR(alpha);r = -7;
	//roll = DOR(gamma);
	//alpha = angles[13]+angles[12];
	//Translation[13] = translate(-r*sin(roll),r*cos(pitch),r*sin(pitch))*Translation[12];
	//Rotatation[13] = rotate(alpha,1,0,0);
	//Models[13] = Translation[13]*Rotatation[13]*Models[13];

	//pitch = DOR(alpha); r = -5;
	////b = DOR(angles[13]);
	//roll = DOR(gamma);
	//Translation[14] = translate(-(r+2)*sin(roll),r*cos(pitch),r*sin(pitch)-1)*Translation[13];
	//Rotatation[14] = rotate(alpha,1,0,0);
	//Models[14] = Translation[14]*Rotatation[14]*Models[14];
	////=============================================================
	////右腳
	//alpha = angles[15] = -angles[12];
	//gamma = -10;
	//Rotatation[15] = rotate(alpha ,1,0,0)*rotate(gamma ,0,0,1);
	//Translation[15] =translate(-1.8,-4.5,0);
	//Models[15] = Translation[15]*Rotatation[15]*Models[15];

	//angles[16] = angles[13];
	//pitch = DOR(alpha);r = -7;
	//roll = DOR(gamma);
	//alpha = angles[16]+angles[15];
	//Rotatation[16] = rotate(alpha,1,0,0);
	//Translation[16] = translate(-r*sin(roll),r*cos(pitch),r*sin(pitch))*Translation[15];
	//Models[16] = Translation[16]*Rotatation[16]*Models[16];

	//pitch = DOR(alpha); r = -5;
	////b = DOR(angles[16]);
	//roll = DOR(gamma);
	//alpha = angles[15]+angles[16];
	//Translation[17] = translate(-(r+2)*sin(roll),r*cos(pitch),r*sin(pitch)-0.5)*Translation[16];
	//Rotatation[17] = rotate(alpha,1,0,0);
	//Models[17] = Translation[17]*Rotatation[17]*Models[17];
	////=============================================================
	}
}

void load2Buffer(char* obj,int i){
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals; // Won't be used at the moment.
	std::vector<unsigned int> materialIndices;

	bool res = loadOBJ(obj, vertices, uvs, normals,faces[i],mtls[i]);
	if(!res) printf("load failed\n");

	//glUseProgram(program);

	glGenBuffers(1,&VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),&vertices[0],GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	//(buffer type,data起始位置,data size,data first ptr)
	//vertices_size[i] = glm_model->numtriangles;
	
	//printf("vertices:%d\n",vertices_size[);

	glGenBuffers(1,&uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1,&nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}

mat4 translate(float x,float y,float z){
	vec4 t = vec4(x,y,z,1);//w = 1 ,則x,y,z=0時也能translate
	vec4 c1 = vec4(1,0,0,0);
	vec4 c2 = vec4(0,1,0,0);
	vec4 c3 = vec4(0,0,1,0);
	mat4 M = mat4(c1,c2,c3,t);
	return M;
} 

mat4 scale(float x,float y,float z){
	vec4 c1 = vec4(x,0,0,0);
	vec4 c2 = vec4(0,y,0,0);
	vec4 c3 = vec4(0,0,z,0);
	vec4 c4 = vec4(0,0,0,1);
	mat4 M = mat4(c1,c2,c3,c4);
	return M;
}

mat4 rotate(float angle,float x,float y,float z){
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r)+(1-cos(r))*x*x,(1-cos(r))*y*x+sin(r)*z,(1-cos(r))*z*x-sin(r)*y,0);
	vec4 c2 = vec4((1-cos(r))*y*x-sin(r)*z,cos(r)+(1-cos(r))*y*y,(1-cos(r))*z*y+sin(r)*x,0);
	vec4 c3 = vec4((1-cos(r))*z*x+sin(r)*y,(1-cos(r))*z*y-sin(r)*x,cos(r)+(1-cos(r))*z*z,0);
	vec4 c4 = vec4(0,0,0,1);
	M = mat4(c1,c2,c3,c4);
	return M;
}

void Keyboard(unsigned char key, int x, int y){
	switch(key){
	case '1':
		angle += 5;
		if(angle>=360) angle = 0;
		printf("beta:%f\n",angle);
		break;
	case '2':
		angle -= 5;
		if(angle<=0) angle = 360;
		printf("beta:%f\n",angle);
		break;
	case 'w':
		eyedistance -= 0.2;
		break;
	case 's':
		eyedistance += 0.2;
		break;
	case 'a':
		eyeAngley -=10;
		break;
	case 'd':
		eyeAngley +=10;
		break;
	case 'r':
		angles[1] -= 5; 
		if(angles[1] == -360) angles[1] = 0;
		movey = 0;
		movex = 0;
		break;
	case 't':
		angles[2] -= 5;
		if(angles[2] == -360) angles[2] = 0;
		movey = 0;
		movex = 0;
		break;
	case 'b':
		PlaySound(TEXT("bear sound.wav"), NULL, SND_SYNC);
		break;
	case 'e':
		break;
	}
	glutPostRedisplay();
}

void menuEvents(int option){}

void ActionMenuEvents(int option){
	switch(option){
	case 0:
		action = IDLE;
		break;
	case 1:
		action = WALK;
		break;
	case 2:
		action = GUARD;
		break;
	case 3:
		action = THINK;
		break;
	case 4:
		action = BEAR;
		break;
	case 5:
		action = HUG;
		break;
	case 6:
		PlaySound(TEXT("bear sound.wav"), NULL, SND_SYNC);
		break;
	}
}

void ModeMenuEvents(int option){
	switch(option){
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}

void ShaderMenuEvents(int option){
	switch (option) {
	case 0:
		shader = CLOSE;
		glUniform1f(glGetUniformLocation(program, "shader"), 0);
		break;
	case 1:
		shader = OPEN;
		glUniform1f(glGetUniformLocation(program, "shader"), 1);
		break;
	case 2:
		shader = PIXEL;
		glUniform1f(glGetUniformLocation(program, "shader"), 2);
		break;
	case 3:
		glUniform1f(glGetUniformLocation(program, "shader"), 3);
		break;
	}
}

void Timer (int time)
{
	rotateShader_t += 0.01;
	if (rotateShader_t >= 1)
		rotateShader_t = 0;

	if (action == WALK) {

		PandaAnglesDirection[A_HEAD][0] = 0;
		PandaAnglesDirection[A_HEAD][1] = 1;
		PandaAnglesDirection[A_HEAD][2] = 0;

		PandaAnglesDirection[A_LEFT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_LEFT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_LEFT_UP_LEG][2] = 0;

		PandaAnglesDirection[A_RIGHT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_RIGHT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_RIGHT_UP_LEG][2] = 0;

		if (Cal_moveTime == moveTime[MoveNum])
		{
			Cal_moveTime = 0;
			MoveNum++; // next MOVE
			if (MoveNum >= MOVE) MoveNum = 0;

			for (int i = 0; i < ELEMENTS; i++) 
			{
				DeltaAngles[i] = (WalkAngles[MoveNum][i] - PandaAngles[i]) / moveTime[MoveNum];
				DeltaTranslate_Y[i] = (WalkTranslate_Y[MoveNum][i] - PandaTranslate_Y[i]) / moveTime[MoveNum];
				DeltaTranslate_Z[i] = (WalkTranslate_Z[MoveNum][i] - PandaTranslate_Z[i]) / moveTime[MoveNum];
			}				
		}
		if (Cal_moveTime < moveTime[MoveNum])
		{
			Cal_moveTime++;
			for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] += DeltaAngles[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Y[i] += DeltaTranslate_Y[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Z[i] += DeltaTranslate_Z[i];
		}
	}

	if (action == IDLE)
	{
		for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] = IdleAngles[i];
		for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Y[i] = 0;
		for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Z[i] = 0;
	}

	if (action == GUARD) 
	{
		PandaAnglesDirection[A_LEFT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_LEFT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_LEFT_UP_LEG][2] = 0;

		PandaAnglesDirection[A_RIGHT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_RIGHT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_RIGHT_UP_LEG][2] = 0;

		if (Cal_moveTime == moveTime[MoveNum])
		{
			Cal_moveTime = 0;
			MoveNum++; // next MOVE
			if (MoveNum >= MOVE) MoveNum = 3;

			for (int i = 0; i < ELEMENTS; i++)
			{
				DeltaAngles[i] = (GuardAngles[MoveNum][i] - PandaAngles[i]) / moveTime[MoveNum];
				DeltaTranslate_Y[i] = (GuardTranslate_Y[MoveNum][i] - PandaTranslate_Y[i]) / moveTime[MoveNum];
				DeltaTranslate_Z[i] = (GuardTranslate_Z[MoveNum][i] - PandaTranslate_Z[i]) / moveTime[MoveNum];
			}
		}
		if (Cal_moveTime < moveTime[MoveNum])
		{
			Cal_moveTime++;
			for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] += DeltaAngles[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Y[i] += DeltaTranslate_Y[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Z[i] += DeltaTranslate_Z[i];
		}
	}

	if (action == THINK)
	{
		
		PandaAnglesDirection[A_HEAD][0] = 1;
		PandaAnglesDirection[A_HEAD][1] = 0;
		PandaAnglesDirection[A_HEAD][2] = 0;

		PandaAnglesDirection[A_LEFT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_LEFT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_LEFT_UP_LEG][2] = 0;

		PandaAnglesDirection[A_RIGHT_UP_LEG][0] = 1;
		PandaAnglesDirection[A_RIGHT_UP_LEG][1] = 0;
		PandaAnglesDirection[A_RIGHT_UP_LEG][2] = 0;

		if (Cal_moveTime == moveTime[MoveNum])
		{
			Cal_moveTime = 0;
			MoveNum++; // next MOVE
			if (MoveNum >= MOVE) MoveNum = 3;

			for (int i = 0; i < ELEMENTS; i++)
			{
				DeltaAngles[i] = (ThinkAngles[MoveNum][i] - PandaAngles[i]) / moveTime[MoveNum];
				DeltaTranslate_Y[i] = (ThinkTranslate_Y[MoveNum][i] - PandaTranslate_Y[i]) / moveTime[MoveNum];
				DeltaTranslate_Z[i] = (ThinkTranslate_Z[MoveNum][i] - PandaTranslate_Z[i]) / moveTime[MoveNum];
			}
		}
		if (Cal_moveTime < moveTime[MoveNum])
		{
			Cal_moveTime++;
			for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] += DeltaAngles[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Y[i] += DeltaTranslate_Y[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Z[i] += DeltaTranslate_Z[i];
		}
	}

	if (action == BEAR)
	{

		PandaAnglesDirection[A_HEAD][0] = 1;
		PandaAnglesDirection[A_HEAD][1] = 0;
		PandaAnglesDirection[A_HEAD][2] = 0;		

		PandaAnglesDirection[A_LEFT_UP_LEG][0] = 0.8;
		PandaAnglesDirection[A_LEFT_UP_LEG][1] = 0.5;
		PandaAnglesDirection[A_LEFT_UP_LEG][2] = 0;

		PandaAnglesDirection[A_RIGHT_UP_LEG][0] = -0.8;
		PandaAnglesDirection[A_RIGHT_UP_LEG][1] = 0.5;
		PandaAnglesDirection[A_RIGHT_UP_LEG][2] = 0;	

		if (Cal_moveTime == moveTime[MoveNum])
		{
			Cal_moveTime = 0;
			MoveNum++; // next MOVE
			if (MoveNum >= MOVE) MoveNum = 3;

			for (int i = 0; i < ELEMENTS; i++)
			{
				DeltaAngles[i] = (BearAngles[MoveNum][i] - PandaAngles[i]) / moveTime[MoveNum];
				DeltaTranslate_Y[i] = (BearTranslate_Y[MoveNum][i] - PandaTranslate_Y[i]) / moveTime[MoveNum];
			}
		}
		if (Cal_moveTime < moveTime[MoveNum])
		{
			Cal_moveTime++;
			for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] += DeltaAngles[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Y[i] += DeltaTranslate_Y[i];
		}
	}

	if (action == HUG) {

		if (Cal_moveTime == moveTime[MoveNum])
		{
			Cal_moveTime = 0;
			MoveNum++; // next MOVE
			if (MoveNum >= MOVE) MoveNum = 3;

			for (int i = 0; i < ELEMENTS; i++)
			{
				DeltaAngles[i] = (HugAngles[MoveNum][i] - PandaAngles[i]) / moveTime[MoveNum];
				DeltaTranslate_Z[i] = (HugTranslate_Z[MoveNum][i] - PandaTranslate_Z[i]) / moveTime[MoveNum];
			}
		}
		if (Cal_moveTime < moveTime[MoveNum])
		{
			Cal_moveTime++;
			for (int i = 0; i < ELEMENTS; i++) PandaAngles[i] += DeltaAngles[i];
			for (int i = 0; i < ELEMENTS; i++) PandaTranslate_Z[i] += DeltaTranslate_Z[i];
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}

GLuint LoadTexture(string filename)
{
	int imgWidth, imgHeight;
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char * image = SOIL_load_image(filename.c_str(), &imgWidth, &imgHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(glGetUniformLocation(program, "sceneTex"), 0);

	glUniform1f(glGetUniformLocation(program, "rt_w"), imgWidth);
	glUniform1f(glGetUniformLocation(program, "rt_h"), imgHeight);

	glUniform1f(glGetUniformLocation(program, "pixel_w"), 15.0f);
	glUniform1f(glGetUniformLocation(program, "pixel_h"), 15.0f);

	return textureID;

}
