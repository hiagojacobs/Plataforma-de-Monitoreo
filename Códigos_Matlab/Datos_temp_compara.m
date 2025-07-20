% --- Configuración inicial ---
puerto = "COM41";           % ⚠️ Cambiá por tu puerto real
baud = 115200;
s = serialport(puerto, baud);
flush(s);

% Inicializar buffers
maxPuntos = 300;
tiempo = nan(1, maxPuntos);
DS18B20 = nan(1, maxPuntos);
DHT11 = nan(1, maxPuntos);
t0 = datetime('now');

refFluke = 27.3;

% Preparar la figura
figure;
h1 = plot(nan, nan, '-or', nan, nan, '-ob', 'LineWidth', 1.5); hold on;
hRef = plot(nan, nan, '-og', 'LineWidth', 1.5);  % Línea verde con círculos (no punteada)
legend("DS18B20", "DHT11", "Fluke", 'Location', 'northeast');
xlabel("Tiempo (s)");
ylabel("Temperatura (°C)");
title("Comparación de Lectura de Temperatura");
ylim([20 30]);  % Escala reducida
grid on;

% --- Loop principal ---
while true
    if s.NumBytesAvailable > 0
        linea = readline(s);
        datos = str2num(strtrim(linea));  % Convierte texto en números

        if numel(datos) == 2
            t = seconds(datetime('now') - t0);
            tiempo = [tiempo(2:end), t];
            DS18B20 = [DS18B20(2:end), datos(1)];
            DHT11 = [DHT11(2:end), datos(2)];

            set(h1(1), 'XData', tiempo, 'YData', DS18B20);
            set(h1(2), 'XData', tiempo, 'YData', DHT11);
            set(hRef, 'XData', tiempo, 'YData', refFluke * ones(1, maxPuntos));

            drawnow;
        end
    end
end
