void ConmutadorDePotencia() {
	if (_EstadoConmutador) {
		BuzzerConmutador();
		Conmutador(20, 1000, 160, 60); //Parametros
	}
}

void Conmutador(int valTiempoON, int valTiempoOFF, int valFrecuenciaAutoinduccion, int valCicloUtilAutoinduccion) {
	int valTiempoAutoInduccion;
	int Repeticiones;
	valTiempoAutoInduccion = (1.0 / valFrecuenciaAutoinduccion) * 1000.0;
	Repeticiones = valTiempoON / valTiempoAutoInduccion;
	for (int tiempo = 0; tiempo < Repeticiones; tiempo++) {
		AutoInduccion(valTiempoAutoInduccion, valCicloUtilAutoinduccion);
	}
	delayConSirena(valTiempoOFF); //1 segundo
}

void AutoInduccion(int valTiempoAutoInduccion, double valCicloUtil) {
	int mTiempoAlto;
	int mTiempoBajo;
	double mPorcentajeAlto;
	double mPorcentajeBajo;

	mPorcentajeAlto = valCicloUtil / 100.0;
	mPorcentajeBajo = (100.0 - valCicloUtil) / 100.0;
	mTiempoAlto = valTiempoAutoInduccion * mPorcentajeAlto;
	mTiempoBajo = valTiempoAutoInduccion * mPorcentajeBajo;

	digitalWrite(PinConmutador, ON);
	delay(mTiempoAlto);
	digitalWrite(PinConmutador, OFF);
	delay(mTiempoBajo);
}

void delayConSirena(long valTiempoOFF) {
	long TiempoGlobal = millis();
	while ((TiempoGlobal + valTiempoOFF) > millis()) {
		Sirena(_EstadoSirena, valTiempoOFF);
	}
}

void Sirena(bool vEstado, int tiempoAlarma) {
	if (vEstado) { Alarma(500, 800, tiempoAlarma); } //Parametros Empiricos
}

void BuzzerOrden() {
	Alarma(400, 600, 400);
}

void Alarma(int Frecuancia1, int Frecuencia2, int tiempoAlarma) {
	int tiempoDividido = tiempoAlarma / 4;
	tone(PinSirena, Frecuancia1, tiempoDividido);
	delay(tiempoDividido);
	tone(PinSirena, Frecuencia2, tiempoDividido);
	delay(tiempoDividido);
	//Tiempo ON Sirena = 1 segundo
}

void RecibirOrdenes() {
	if (Wifi.available() > 0) {
		delay(1000);
		
		if (Wifi.find("+IPD,")) {
			
			ConexionID = Wifi.read() - 48;

			int EstadoWifi = 0;
			if (Wifi.find("Estado=")) {
				EstadoWifi = Wifi.read() - 48;
			}
			switch (EstadoWifi) {
			case 0:
				_EstadoSirena = OFF;
				_EstadoConmutador = OFF;
				OrdenRecibida();
				_Evento = sistemaOFF;
				ActualizarDB();
				break;
			case 1:
				OrdenRecibida();
				_EstadoConmutador = ON;
				_Evento = sistemaON;
				ActualizarDB();
				break;
			case 2:
				OrdenRecibida();
				if (ConexionWifi()) {
					Responder(PaginaConfirmacionConexion());
				}
				break;
			}
		}
		else {
			Mensaje("Msj Extra�o");
		}
	}
}

void OrdenRecibida(){
	Responder("OK");
	BuzzerOrden(); // aqui se tarda 1 segundo
	Mensaje("Orden Recibida");
	vaciar();
}

void ActualizarDB() {
	ModoClient();
	Enviar((String)_Evento);
	ModoServidor();
	Mensaje("Base de Datos Actualizada");
}

void GuardarMemoria() {
	_direccionMemoria++;
	EEPROM.write(_direccionMemoria, _Evento);
	_Evento = Ninguno;
	Mensaje("Evento Almacenado en Memoria");
}

void LeerMemoria() {
	_direccionMemoria = 0;
	_Evento = EEPROM.read(_direccionMemoria);
	if (_Evento == 0) {
		_Evento = FuenteON;
		ActualizarDB();
	}
	else {
		while (_Evento != 0) {
			_Evento = EEPROM.read(_direccionMemoria);
			ActualizarDB();
			_direccionMemoria++;
		}
		_direccionMemoria = -1;
	}
}

void BuzzerConmutador() {
	if (_EstadoSirena == OFF && _EstadoConmutador == ON) {
		digitalWrite(PinBuzzer, ON);
		delay(100);
		digitalWrite(PinBuzzer, OFF);
		delay(100);
	}
}

void IniciarVariables() {
	_EstadoSirena = OFF;
	_EstadoConmutador = OFF;
	_direccionMemoria = -1;
	_Evento = Ninguno;
	_EstadoFuente = digitalRead(PinFuente);
	_debug = true;	//<--------- DEPURADOR
}