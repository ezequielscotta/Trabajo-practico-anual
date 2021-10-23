/*Inclusion de bibliotecas*/
#include <iostream>
#include <conio.h>
#include <string.h>
using namespace std;

/*Declaracion de estructuras de datos*/
struct RegistroUsuario{
	int UsuarioID;//99999
	int FechaCreacion;//AAAAMMDD
	bool Activo;
	float TotalImporteCompras;
	char eMail[50];
};

struct Compra{
	int CompraID;//99999
	char FechaHora[14];//AAAAMMDDHH:MM
	float Monto;
	int UsuarioID;//99999
	int NroArticulo;
};

/*Declaracion de prototipos de las funciones implementadas*/
bool VerifChar(char []);
void LevantarUsuarios(char [], RegistroUsuario []);
bool BuscarID(RegistroUsuario [], int);
void CargarUsuario(char [], RegistroUsuario []);
void ActividadUsuario(RegistroUsuario[]);
void BuscarPorID(RegistroUsuario Usu[]);
void BuscarPoreMail(RegistroUsuario []);
void MostrarComprasRealizadasPorUsuario(char [], RegistroUsuario []);
void OrdenarArchivoPorTotalImporte(RegistroUsuario []);
void MostrarUsuarios(RegistroUsuario []);
void ProcesarUnLoteDeCompras(char [], RegistroUsuario []);
bool VerifCharsinhora(char []);
void Reportehtml(char[]);
void ReporteCSV(char []);
int Menu(void);

/*Programa principal*/
int main(){
	/*Declaracion e inicializacion de variables a utilizar*/
	char DirUsB[]="Usuarios.bin", DirCB[]="procesados.bin", Eleccion; RegistroUsuario Vec[1000];bool Flag=false;
	for(int i=0;i<1000;i++){
		Vec[i].UsuarioID=-1;/*El -1 es mi valor "vacio"*/
		Vec[i].TotalImporteCompras=0;/*Inicio en cero para ir acumulando los importes*/
	}
	do{
		Eleccion=Menu();/*Menu devuelve la eleccion que analizamos en el switch para implementar la eleccion*/

		switch(Eleccion){
			case '1':
				system("CLS");
				LevantarUsuarios(DirUsB, Vec);
				break;
			case '2':
				system("CLS");
				CargarUsuario(DirUsB, Vec);
				getchar();
				break;
			case '3':
				system("CLS");
				ActividadUsuario(Vec);
				break;
			case '4':
				do{
					system("CLS");
					cout<<"\n\n\t\t -------- MENU DE BUSQUEDA ---------";
					cout<<"\n\t\t 1. Buscar Por ID.";
					cout<<"\n\t\t 2. Buscar Por eMail.";
					cout<<"\n\t\t Eleccion: ";
					Eleccion=getch();
				}while(Eleccion!='1' && Eleccion!='2');
				switch(Eleccion){
					case '1':
						system("CLS");
						BuscarPorID(Vec);
					break;
					case '2':
						system("CLS");
						BuscarPoreMail(Vec);
					break;
				}
				getchar();
				cout<<"\n\n\t\t Sistema en pausa presione una tecla para continuar... ";
				break;
			case '5':
				system("CLS");
				OrdenarArchivoPorTotalImporte(Vec);
				MostrarUsuarios(Vec);
				cout<<"\n\n\t\t Sistema en pausa presione una tecla para continuar... ";
				getch();
				break;
			case '6':
				ProcesarUnLoteDeCompras(DirCB, Vec);
				system("CLS");
				break;
			case '7':
				system("CLS");
				MostrarComprasRealizadasPorUsuario(DirCB, Vec);
				cout<<"\n\n\t\t Sistema en pausa presione una tecla para continuar... ";
				getch();
				break;
			case '8':
				system("CLS");
				Reportehtml(DirCB);
				cout<<"\n\n\t\t Sistema en pausa presione una tecla para continuar... ";
				getch();
				break;
			case '9':
				system("CLS");
				ReporteCSV(DirCB);
				cout<<"\n\n\t\t Sistema en pausa presione una tecla para continuar... ";
				getch();
				break;
		}
	}while(Eleccion!='Q' && Eleccion!='q');

	/*Al finalizar el ciclo vuelvo a cargar los datos
	actualizados en el archivo Usuarios.bin*/
	FILE *E;
	if(Vec[0].UsuarioID!=-1)
		if(E=fopen(DirUsB,"wb"))
			for(int i=0;i<1000;i++){
				if(Vec[i].UsuarioID!=-1){
					fwrite(&Vec[i], sizeof(RegistroUsuario), 1, E);
				}else
					i=1000;
			}
	fclose(E);

	return 0;
}

/*Desarrollo de las funciones*/

bool VerifChar(char Num[]){
	/*Declaracion e inicializacion de variables a utlizar en esta funcoin*/
	char AneoMin[]="1900", DiaMin[]="01", DiaMax[]="31", MesMin[]="01", MesMax[]="12";		/*Esta funcion es utlizada para procesar un lote de compra*/
	char HoraMin[]="00", HoraMax[]="23", MinuMin[]="00", MinuMax[]="59";					/*ya que ingresamos char AAAAMMDDHH:MM es necesario verificar*/
																							/*que la fecha y hora ingresada sea informacion valida*/
	char Aneo[]={Num[0],Num[1],Num[2],Num[3]}, Mes[]={Num[4],Num[5]}, Dia[]={Num[6],Num[7]};/*de ser informacion valida retorna true y de lo contrario false*/
	char Hora[]={Num[8],Num[9]}, Minu[]={Num[11],Num[12]};

	/*Verificamos que lo ingresado este dentro de los rangos establecidos*/
	if(strcmp(AneoMin,Aneo)>0 || strcmp(DiaMin,Dia)>0 || strcmp(DiaMax,Dia)<0 || \
	   strcmp(HoraMin,Hora)>0 || strcmp(HoraMax,Hora)<0 || Num[10]!=':' || \
	   strcmp(MinuMin,Minu)>0 || strcmp(MinuMax,Minu)<0)
		return false;
	else
		return true;
}

void ProcesarUnLoteDeCompras(char Dir[], RegistroUsuario Vec[]){
	/*Declaracion de variables a utilizar*/
	FILE *E; Compra Com; float Aux;															/*En esta funcion procesamos la compra asi como tambien*/
	system("CLS");																			/*validamos que los datos ingresados sean correctos de modo tal*/
																							/*que exista el usuario que realiza la compra que las fechas sean*/
	if(E=fopen(Dir,"ab")){
		cout<<endl;
		/*Validacion y carga del ID*/														/*validas, los montos mayores a 0, etc. Y finalizamos cargando la*/
		do{																					/*compra en el archivo procesados.bin de modo tal que cada lote de*/
			cout<<"\n\t Ingrese el ID de la compra (0 a 99999): ";							/*compra quedara registrado si todos los datos ingresados fueron correctos*/
			cin>>Aux;
			Com.CompraID=Aux;
			if(Com.CompraID<0 || Com.CompraID>99999 || Com.CompraID-Aux!=0)
				cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Com.CompraID<0 || Com.CompraID>99999 || Com.CompraID-Aux!=0);
		/*Validacion y carga de la fecha y hora*/
		do{
			cout<<"\n\t Ingrese la fecha y hora (AAAAMMDDHH:MM): ";
			cin>>Com.FechaHora;
			if(VerifChar(Com.FechaHora)==false)
				cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(VerifChar(Com.FechaHora)==false);
		/*Validacion y carga del monto de compra*/
		do{
			cout<<"\n\t Ingrese el monto de la compra: ";
			cin>>Com.Monto;
			if(Com.Monto<=0)
				cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Com.Monto<=0);
		/*Validacion de Nro de articulo, positivo y entero*/
		do{
			cout<<"\n\t Ingrese el numero de articulo: ";
			cin>>Aux;
			Com.NroArticulo=Aux;
			if(Aux<0 || Aux-Com.NroArticulo!=0)
				cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Aux<0 || Aux-Com.NroArticulo!=0);
		int Encontrado=true;
		/*Verifico que el usuario sea un usuario existente*/
		do{
				cout<<"\n\t Ingrese los siguientes datos del usuario: \n";
				cout<<"\n\t Ingrese el ID del usuario (numero entero de 0 a 99999): ";
				cin>>Aux;
				Com.UsuarioID=Aux;
				if(Com.UsuarioID>=0 || Com.UsuarioID<=99999 || Com.UsuarioID-Aux==0)
					Encontrado=BuscarID(Vec, Com.UsuarioID);
				if(Com.UsuarioID<0 || Com.UsuarioID>99999 || Com.UsuarioID-Aux!=0 || Encontrado==false){
					cout<<"\n\n\t ERROR: ID invalido, porfavor intentelo de nuevo.";
				}
			}while(Encontrado==false);
		/*Atualizo el importe total en el registro de usuarios*/
		fwrite(&Com, sizeof(Compra), 1, E);
		for(int i=0;i<1000;i++){
			if(Vec[i].UsuarioID==Com.UsuarioID){
				Vec[i].TotalImporteCompras+=Com.Monto;
				i=1000;
			}
		}
	}else{
		cout<<"\n\n\t\t No hay datos cargados o no se pudo abrir el archivo.";
	}
	fclose(E);
}

void OrdenarArchivoPorTotalImporte(RegistroUsuario Vec[]){
	/*Declaro variable auxiliar que utilizare*/				/*Es un simple algoritmo de ordenamiento por burbujeo*/
	RegistroUsuario Aux;									/*Ordenamos la informacion cargada al vector por Importe total*/
	for(int i=0;i<sizeof(Vec);i++){
		for(int j=0;j<sizeof(Vec);j++){
			if(Vec[j].TotalImporteCompras<Vec[j+1].TotalImporteCompras && Vec[j].UsuarioID!=-1 && Vec[j+1].UsuarioID!=-1){
				Aux=Vec[j];
				Vec[j]=Vec[j+1];
				Vec[j+1]=Aux;
			}
		}
	}
}

int Menu(void){
								/*Menu de opciones usada en el main, devuelve el valor de la eleccion*/
	int Eleccion;				/*verificando que este dentro de los valores establecidos*/

	do{
			system("CLS");
			cout<<"\n\n\t\t -------------- MENU --------------";
			cout<<"\n\t\t 1 - Levantar Usuarios.";
			cout<<"\n\t\t 2 - Cargar Nuevo Usuario.";
			cout<<"\n\t\t 3 - Actividad De Un Usuario.";
			cout<<"\n\t\t 4 - Buscar Usuario.";
			cout<<"\n\t\t 5 - Listar Usuarios.";
			cout<<"\n\t\t 6 - Procesar Un Lote De Compras.";
			cout<<"\n\t\t 7 - Mostrar Compras Realizadas Por Un Usuario.";
			cout<<"\n\t\t 8 - Mostrar todas las compras realizadas entre dos fechas \
					\n\t\t    en un reporte escrito en formato html y el total de las compras.";
			cout<<"\n\t\t 9 - Mostrar todas las compras realizadas entre dos fechas \
					\n\t\t    en un reporte escrito en formato CSV y el total de las compras.";
			cout<<"\n\t\t Q - Finalizar Jornada.";
			cout<<"\n\n\t\t Su eleccion es: ";
			Eleccion=getch();
		}while(Eleccion!='1' && Eleccion!='2' && Eleccion!='3' && Eleccion!='4' && Eleccion!='5' && \
			Eleccion!='6' && Eleccion!='7' && Eleccion!='q' && Eleccion!='Q' && \
			Eleccion!='8' && Eleccion!='9');
	return Eleccion;
}

void LevantarUsuarios(char Dir[], RegistroUsuario Vec[]){
	/*Declaracion e inicializacion de varianles a utilizar*/
	FILE *E; int i=0;											/*Esta funcion toma los datos el archivo Usuarios.bin*/
																/*y los guarda dentro de un vector para su manejo posterior*/
	if(E=fopen(Dir,"rb")){
		fread(&Vec[i], sizeof(RegistroUsuario), 1, E);
		while(i<1000 && !feof(E)){
			i++;
			fread(&Vec[i], sizeof(RegistroUsuario), 1, E);
		}
	}else{
		cout<<"\n\n\t\t No se pudo abrir el archivo.";
	}
	fclose(E);
}

void MostrarUsuarios(RegistroUsuario Usu[]){
	/*Declaracion e inicializacion de varianles a utilizar*/									/*Esta funcion muestra el registro de usuraios*/
    int i=0, j=1;																				/*siempre y cuando el usuario no haya sido desactivado*/
																								/*En caso de no poder mostrar usuarios indica*/
    if(Usu[i].UsuarioID!=-1){																	/*que no hay datos cargados*/
        cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        while(i<1000){
        	if(Usu[i].Activo==true && Usu[i].UsuarioID!=-1){
	            cout<<"\n\n\n\t Datos correspondientes al RegistroUsuario Nro "<<j<<": \n";
				cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
				cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
				cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
				cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
				cout<<"\n\t eMail: "<<Usu[i].eMail;
            	j++;
            }
            i++;
        }
    }else
    	cout<<"\n\n\t No Hay datos cargados";
}

void MostrarComprasRealizadasPorUsuario(char Dir[],RegistroUsuario Vec[]){
	/*Declaracion e inicializacion de variables*/											/*Esta funcion pide el ID del usuario validando la informacion*/
	FILE *E; Compra Com; float Aux;															/*ingresada, busca el ID del usuario en caso afirmativo muestra*/
    int i=0, ID; bool Encontrado=false;

    do{
			do{
				cout<<"\n\t Ingrese el ID del usuario (numero entero de 0 a 99999): ";
				cin>>Aux;
				ID=Aux;
				if(ID<0 || ID>99999 || ID!=Aux)
					cout<<"\n\n\t ERROR: ID invalido, porfavor intentelo de nuevo.";
				else
					Encontrado=BuscarID(Vec, ID);
				if(Encontrado==false)
					cout<<"\n\t\t Usuario inexistente.";
			}while(Encontrado==false);
	}while(ID<0 || ID>99999 || ID!=Aux);
    /*Si se puede abrir el archivo hace esto*/
    if(E=fopen(Dir,"rb")){
        fread(&Com, sizeof(Compra), 1, E);
        cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        cout<<"\n\n\n\t Datos correspondientes al usuario con el ID "<<ID<<": \n";
        while(!feof(E)){
        	if(Com.UsuarioID==ID){
				cout<<"\n\n\t CompraID: "<<Com.CompraID;
				cout<<"\n\t Fecha y hora: "<<Com.FechaHora;
				cout<<"\n\t Monto: "<<Com.Monto;
				cout<<"\n\t Nro de Articulo: "<<Com.NroArticulo;
        	    fread(&Com, sizeof(Compra), 1, E);
            	i++;
            }else
            	fread(&Com, sizeof(Compra), 1, E);
        }
        fclose(E);
    }else
    	cout<<"\n\n\t No se pudo abrir el archivo";
}

void BuscarPoreMail(RegistroUsuario Usu[]){

	char eMail[50]; int i=1;
	if(Usu[0].UsuarioID!=-1){
		cout<<"\n\n\t Ingrese el eMail del RegistroUsuario: ";
		cin>>eMail;
		getchar();
        cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        while(i-1<1000 && (strcmp(eMail, Usu[i-1].eMail))!=0){
            if(Usu[i-1].Activo==true)
            	i++;
        }
        if((strcmp(eMail, Usu[i-1].eMail))==0 && Usu[i-1].Activo==true){
        	cout<<"\n\n\n\t Datos correspondientes al usuario Nro "<<i<<": \n";i--;
			cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
			cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
			cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
			cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
			cout<<"\n\t eMail: "<<Usu[i].eMail;
		}else
			cout<<"\n\n\t No se encontro usuario con el eMail >> "<<eMail<<"\n";
    }else
    	cout<<"\n\n\t No se hay datos cargados";
}

void BuscarPorID(RegistroUsuario Usu[]){
	int i=1, ID;
	if(Usu[0].UsuarioID!=-1){
		cout<<"\n\n\t Ingrese el ID del Usuario: ";
		cin>>ID;
		getchar();
        cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        while(i-1<1000 && ID!=Usu[i-1].UsuarioID){
            if(Usu[i-1].Activo==true);
            	i++;
        }
        if(ID==Usu[i-1].UsuarioID && Usu[i-1].Activo==true){
        	cout<<"\n\n\n\t Datos correspondientes al usuario Nro "<<i<<": \n";i--;
			cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
			cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
			cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
			cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
			cout<<"\n\t eMail: "<<Usu[i].eMail;
		}else
			cout<<"\n\n\t No se encontro usuario con el ID >> "<<ID<<"\n";
    }else
    	cout<<"\n\n\t No se pudo abrir el archivo";
}

void ActividadUsuario(RegistroUsuario Usu[]){
	char Eleccion;
	int i=0;
	do{
		do{
			system("CLS");
			cout<<"\n\n\t\t ---------- MENU  ESTADO ----------";
			cout<<"\n\t\t 1 - Cambiar estado del usuario por eMail.";
			cout<<"\n\t\t 2 - Cambiar estado del RegistroUsuario por ID.";
			cout<<"\n\t\t Q - Quit (Salir)";
			cout<<"\n\n\t\t Su eleccion es: ";
			Eleccion=getch();
		}while(Eleccion!='1' && Eleccion!='2' && Eleccion!='q' && Eleccion!='Q');

		switch(Eleccion){
			case '1':
				system("CLS");
				char eMail[50];
				if(Usu[i].UsuarioID!=-1){
					cout<<"\n\n\t Ingrese el eMail del usuario: ";
					cin>>eMail;
    	    		cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        			while(i<1000 && (strcmp(eMail, Usu[i].eMail))!=0){
						i++;
        			}
		       		if((strcmp(eMail, Usu[i].eMail))==0){
        				cout<<"\n\n\n\t Datos correspondientes al usuario Nro "<<i<<": \n";
						cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
						cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
						cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
						cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
						cout<<"\n\t eMail: "<<Usu[i].eMail;
						do{
							cout<<"\n\n\t Seleccione el estado del usuario:";
							cout<<"\n\t 1 - Desactivar.";
							cout<<"\n\t 9 - Activar.";
							cout<<"\n\t Su eleccion es: ";
							Eleccion=getch();
						}while(Eleccion!='1' && Eleccion!='9');
						if(Eleccion=='1'){
							Usu[i].Activo=false;
						}else{
							Usu[i].Activo=true;
						}
					}else
						cout<<"\n\n\t No se encontro Usuario con el eMail >> "<<eMail<<"\n";
						getchar();
    			}else
    				cout<<"\n\n\t No hay elementos cargados";
				getchar();
				break;
			case '2':
				system("CLS");
				int ID; i=0;
				if(Usu[i].UsuarioID!=-1){
					cout<<"\n\n\t Ingrese el ID del usuario: ";
					cin>>ID;
    	    		cout<<"\n\n\t -- Se encontraron los siguientes datos -- \n";
        			while(i<1000 && ID!=Usu[i].UsuarioID){
						i++;
        			}
		       		if(ID==Usu[i].UsuarioID){
        				cout<<"\n\n\n\t Datos correspondientes al usuario Nro "<<i<<": \n";
						cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
						cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
						cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
						cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
						cout<<"\n\t eMail: "<<Usu[i].eMail;
						do{
							cout<<"\n\n\t Seleccione el estado del RegistroUsuario:";
							cout<<"\n\t 1 - Desactivar.";
							cout<<"\n\t 9 - Activar.";
							cout<<"\n\t Su eleccion es: ";
							Eleccion=getch();
						}while(Eleccion!='1' && Eleccion!='9');
						if(Eleccion=='1'){
							Usu[i].Activo=false;
							cout<<"\n\n\n\t Datos correspondientes al usuario Nro "<<i<<": \n";
							cout<<"\n\t UsuarioID: "<<Usu[i].UsuarioID;
							cout<<"\n\t Fecha de creacion: "<<Usu[i].FechaCreacion;
							cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Usu[i].Activo;
							cout<<"\n\t Total importe de compras: "<<Usu[i].TotalImporteCompras;
							cout<<"\n\t eMail: "<<Usu[i].eMail;
						}else{
							Usu[i].Activo=true;
						}
					}else
						cout<<"\n\n\t No se encontro usuario con el ID >> "<<ID<<"\n";
    			}else
    				cout<<"\n\n\t No hay elementos cargados";
				getchar();
				break;
		}
	}while(Eleccion!='Q' && Eleccion!='q');
	getch();
}

bool BuscarID(RegistroUsuario Vec[],int ID){
	int i=0;
	while(i<1000 && Vec[i].UsuarioID!=ID){
		i++;
	}
	if(Vec[i].UsuarioID==ID){
		return true;//Lo encontro
	}else{
		return false;//No lo encontro
	}
	return false;
}

void CargarUsuario(char Dir[], RegistroUsuario Vec[]){

	FILE *E; RegistroUsuario Usu; float Aux; bool Encontrado;
	int Dia = Usu.FechaCreacion-(Usu.FechaCreacion/100)*100;
	int Mes = (Usu.FechaCreacion-(Usu.FechaCreacion/10000)*10000)/100;
	int Aneo = Usu.FechaCreacion/10000;
	system("CLS");
	if(E=fopen(Dir,"ab")){
		do{
			do{
				cout<<"\n\n\t Ingrese los siguientes datos del usuario: \n";
				cout<<"\n\t Ingrese el ID del usuario (numero entero de 0 a 99999): ";
				cin>>Aux;
				Usu.UsuarioID=Aux;
				if(Usu.UsuarioID<0 || Usu.UsuarioID>99999 || Usu.UsuarioID-Aux!=0)
					cout<<"\n\n\t ERROR: ID invalido, porfavor intentelo de nuevo.";
				else
					Encontrado=BuscarID(Vec, Usu.UsuarioID);
				if(Encontrado==true)
					cout<<"\n\n\t ID EXISTENTE. Intente de nuevo.";
			}while(Encontrado==true);

		}while(Usu.UsuarioID<0 || Usu.UsuarioID>99999 || Usu.UsuarioID-Aux!=0);

		do{
			cout<<"\n\t Ingrese la fecha de creacion (AAAAMMDD): ";
			cin>>Aux;
			Usu.FechaCreacion=Aux;
			Usu.FechaCreacion;
			Dia = Usu.FechaCreacion-(Usu.FechaCreacion/100)*100;
			Mes = (Usu.FechaCreacion-(Usu.FechaCreacion/10000)*10000)/100;
			Aneo = Usu.FechaCreacion/10000;
			if(Dia<1 || Dia>31 || Mes<1 || Mes>12 || Aneo<1900)//Agregar para numero no entero
				cout<<"\n\n\t ERROR: Fecha invalida, porfavor intentelo de nuevo.";
		}while(Dia<1 || Dia>31 || Mes<1 || Mes>12 || Aneo<1900 || Aux-Usu.FechaCreacion!=0);//Agregar para numero no entero

		cout<<"\n\t Ingrese el eMail (Hasta 50 caracteres): ";
		cin>>Usu.eMail;
		Usu.Activo=true;
		Usu.TotalImporteCompras=0;
		fwrite(&Usu, sizeof(RegistroUsuario), 1, E);

		for(int i=0;i<1000;i++){
			if(Vec[i].UsuarioID==-1){
				Vec[i]=Usu;
				cout<<"\n\n\n\t Datos correspondientes al RegistroUsuario Nro "<<i+1<<": \n";
				cout<<"\n\t UsuarioID: "<<Vec[i].UsuarioID;
				cout<<"\n\t Fecha de creacion: "<<Vec[i].FechaCreacion;
				cout<<"\n\t Actividad (1 -> Activo | 0 -> Inactivo): "<<Vec[i].Activo;
				cout<<"\n\t Total importe de compras: "<<Vec[i].TotalImporteCompras;
				cout<<"\n\t eMail: "<<Vec[i].eMail<<"\n\n";
				i=1000;
			}
		}
	}
	system("PAUSE");
	fclose(E);
}

bool VerifCharsinhora(char Num[]){
	/*Declaracion e inicializacion de variables a utlizar en esta funcion*/
	char AneoMin[]="1900", DiaMin[]="01", DiaMax[]="31", MesMin[]="01", MesMax[]="12";

	char Aneo[]={Num[0],Num[1],Num[2],Num[3]}, Mes[]={Num[4],Num[5]}, Dia[]={Num[6],Num[7]};
	/*Verificamos que lo ingresado este dentro de los rangos establecidos*/
	if(strcmp(AneoMin,Aneo)>0 || strcmp(DiaMin,Dia)>0 || strcmp(DiaMax,Dia)<0 || strcmp(MesMin,Mes)>0 || strcmp(MesMax,Mes)<0)
		return false;
	else
		return true;
}

void Reportehtml(char Dir[]){
    char fechamax[14],fechamin[14]; Compra Aux;
    float Acu; bool Flag=true;
    FILE *f, *E;
    f = fopen("salidahtml.html", "wt"); E = fopen(Dir, "rb");

    system("CLS");
    do{
        do{
        	cout<<"\n\n\t\t Ingrese la fecha maxima a buscar: ";
        	cin>>fechamax;
        	Flag=VerifCharsinhora(fechamax);
        	if(Flag==false)
                cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Flag==false);
        do{
        	cout<<"\n\t\t Ingrese la fecha minima a buscar: ";
	        cin>>fechamin;
	        Flag=VerifCharsinhora(fechamin);
	        if(Flag==false)
                cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Flag==false);

        if(strcmp(fechamin,fechamax)>0)
        	cout<<"\n\n\t\t ERROR: Fecha minima ingresada mayor a fecha maxima";

    }while(strcmp(fechamin,fechamax)>0);
    fprintf(f,"<html><body>\n");
    fprintf(f,"<h1><p style='color:#2F075F';>Reporte de compras efectuadas\n</p></h1>");
    fprintf(f,"<table border=1>\n");
    fprintf(f,"<th>CompraID</th><th>FechaHora</th><th>Monto</th><th>UsuarioID</th><th>Nroarticulo</th><th>Importe Total</th>\n");

    fread(&Aux, sizeof(Compra), 1, E);
    while (!feof(E)){
            if(strcmp (Aux.FechaHora, fechamax)<0 && strcmp (Aux.FechaHora, fechamin)>0){
            	Acu+=Aux.Monto;
	            fprintf(f,"<tr>\n");
    	        fprintf(f,"<td><p style='color:#2F075F';>%d</p></td><td><p style='color:#2F075F';>%s</p></td><td><p \
				style='color:#2F075F';>%f</p></td><td><p style='color:#2F075F';>%d</p></td><td><p style='color:#2F075F';>%d</p></td>\n",Aux.CompraID,Aux.FechaHora,Aux.Monto,Aux.UsuarioID,Aux.NroArticulo);
        	    fprintf(f,"</tr>\n");
            }
    		fread(&Aux, sizeof(Compra), 1, E);
        }
    fprintf(f,"<tr>\n");
    fprintf(f,"<td> </td><td> </td><td> </td><td> </td><td> </td><td><p style='color:#2F075F';>%.2f</p></td>\n",Acu);
	fprintf(f,"</tr>\n");
    fprintf(f, "</table>");
    fprintf(f, "</body>");
    fprintf(f, "</html>");
    fclose(f); fclose(E);
    return;
}

void ReporteCSV(char Dir[]){
    char fechamax[14],fechamin[14]; Compra Aux;
    float Acu; int Flag=true;
    FILE *f, *E;
    f = fopen("salidaexcel.csv", "wt"); E = fopen(Dir, "rb");
    do{
        do{
        	cout<<"\n\n\t\t Ingrese la fecha maxima a buscar: ";
        	cin>>fechamax;
        	Flag=VerifCharsinhora(fechamax);
        	if(Flag==false)
                cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Flag==false);
        do{
        	cout<<"\n\t\t Ingrese la fecha minima a buscar: ";
	        cin>>fechamin;
	        Flag=VerifCharsinhora(fechamin);
	        if(Flag==false)
                cout<<"\n\n\t\t ERROR: El dato ingresado es invalido. Intente de nuevo.";
		}while(Flag==false);

        if(strcmp(fechamin,fechamax)>0)
        	cout<<"\n\n\t\t ERROR: Fecha minima ingresada mayor a fecha maxima";

    }while(strcmp(fechamin,fechamax)>0);

    fprintf(f,"CompraID;FechaHora;Monto;UsuarioID;Nroarticulo;Importe total\n");
	fread(&Aux, sizeof(Compra), 1, E);
    while(!feof(E)){
               if(strcmp(Aux.FechaHora, fechamax)<0 && strcmp (Aux.FechaHora, fechamin)>0){
                    Acu+=Aux.Monto;
                	fprintf(f,"%d;%s;%.2f;%d;%d\n",Aux.CompraID,Aux.FechaHora,Aux.Monto,Aux.UsuarioID,Aux.NroArticulo);
               }
               fread(&Aux, sizeof(Compra), 1, E);
    }
    fprintf(f,"%c;%c;%c;%c;%c;%.2f\n",' ',' ',' ',' ',' ',Acu);
    fclose(f);
    return;
}

