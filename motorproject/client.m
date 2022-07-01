function client()
%   provides a menu for accessing PIC32 motor control functions

% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',"COM3");

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial("COM3", 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120);
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf(['\n\n_____________________________MENU__________________________________\n\n' ...
        '| a: Read Current Sensor(ADC counts)     b: Read Current Sensor(mA) |\n' ...
        '| c: Read Encoder(counts)                d: Read Encoder(deg)       |\n' ...
        '| e: Reset Encoder                       f: Set PWM(-100 to 100)    |\n' ...
        '| g: Set Current Gains                   h: Get Current Gains       |\n' ...
        '| i: Set Position Gains                  j: Get Position Gains      |\n' ...
        '| k: Test Current Control                l: Go to Angle             |\n' ...
        '| m: Load Step Strajectory               n: Load Cubic Trajectory   |\n' ...
        '| o: Execute Trajectory                  p: Unpower the Motor       |\n' ...
        '| q: QUIT Client                         r: Get Mode                |\n'...
        '____________________________________________________________________\n']);
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');

    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);

    % take the appropriate action
    switch selection
        case 'a'                         % example operation
            %n = input('Enter number: '); % get the number to send
            %fprintf(mySerial, '%d\n',n); % send the number
            a = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('I2C count : %d\n\n\n',a);     % print it to the screen


        case 'b'
            b = fscanf(mySerial,'%f');   % get the incremented number back
            fprintf('Motor current : %f\n\n\n',b);     % print it to the screen


        case 'c'
            c = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Encoder count : %d\n\n\n',c);


        case 'd'
            d = fscanf(mySerial,'%f');   % get the incremented number back
            fprintf('Encoder degrees : %d\n\n\n',d);


        case 'e'
            e = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Encoder count after reset : %d\n\n\n',e);


        case 'f'
            pwm = input('What PWM value would you like [-100 to 100]?  '); % get the number to send
            fprintf(mySerial, '%d\n',pwm); % send the number        case 'g'
            f = fscanf(mySerial,'%f');   % get the incremented number back
            if (pwm > 0)
                fprintf('PWM has been set to %d percent in the counterclockwise direction.\n\n\n',f);
            elseif (pwm == 0)
                fprintf('PWM has been set to %d percent and is idling.\n\n\n',f);
            elseif (pwm < 0)
                fprintf('PWM has been set to %d percent in the clockwise direction.\n\n\n',f);
            end


        case 'g'
            kp_cur = input('Enter your desired Kp current gain [recommended: 0.01]:  ');
            ki_cur = input('Enter your desired Ki current gain [recommended: 0.1]:  ');
            fprintf('Sending Kp = %f and Ki = %f to the current controller.\n', kp_cur, ki_cur);
            fprintf(mySerial, '%f\n',kp_cur);
            fprintf(mySerial, '%f\n',ki_cur);


        case 'h'
            g1 = fscanf(mySerial,'%f');
            g2 = fscanf(mySerial,'%f');
            fprintf('The current controller is using Kp = %4.2f and Ki = %4.2f\n\n\n',g1,g2);


        case 'i'
            kp_pos = input('Enter your desired Kp position gain [recommended: 300]:  ');
            ki_pos = input('Enter your desired Ki position gain [recommended: 1]:  ');
            kd_pos = input('Enter your desired Kd position gain [recommended: 300]:  ');
            fprintf('Sending Kp = %f , Ki = %f and Kd = %f to the current controller.\n', kp_pos, ki_pos, kd_pos);
            fprintf(mySerial, '%f\n',kp_pos);
            fprintf(mySerial, '%f\n',ki_pos);
            fprintf(mySerial, '%f\n',kd_pos);

        case 'j'
            j1 = fscanf(mySerial,'%f');
            j2 = fscanf(mySerial,'%f');
            j3 = fscanf(mySerial,'%f');
            fprintf('The current controller is using Kp = %4.2f , Ki = %4.2f and Kd = %4.2f\n\n\n',j1,j2,j3);


        case 'k'
            nsamples = fscanf(mySerial,'%d');       % first get the number of samples being sent
            data = zeros(nsamples,2);               % two values per sample:  ref and actual
            for i=1:nsamples
                data(i,:) = fscanf(mySerial,'%f %f'); % read in data from PIC32; assume ints, in mA
                times(i) = (i-1);                 % 0.2 ms between samples
            end
            if nsamples > 1
                stairs(times,data(:,1:2));            % plot the reference and actual
            else
                fprintf('Only 1 sample received\n')
                disp(data);
            end
            % compute the average error
            score = mean(abs(data(:,1)-data(:,2)));
            fprintf('\nAverage error: %5.1f mA\n',score);
            title(sprintf('Average error: %5.1f mA',score));
            ylabel('Current (mA)');
            xlabel('Time (ms)');


        case 'l'
            l = input('Enter the angle for the motor to move to (deg):\n\n');
            ref_angle = [0,0;3,l];
            ref_angle = genRef(ref_angle, 'cubic');
            fprintf(mySerial,'%d\n',length(ref_angle));
            for angle_index = 1:length(ref_angle)
                fprintf(mySerial, '%d\n',ref_angle(angle_index));
            end
            fprintf('\n\nMotor moving to %d degrees.\n\n' , l);


        case 'm'
            sample_step = [0,0;0.5,100;3,100];
            assignin('base','sample_step',sample_step);

            step_check = 1;
            while (step_check)
                ref_step = input('Enter step trajectory, in sec and degrees [time1, ang1; time2, ang2; ...]:\n\n');
                ref_step = genRef(ref_step, 'step');
                if (length(ref_step)>2001)
                    fprintf('Error: Maximum trajectory time is 10 seconds.\n\n')
                    close
                    step_check = 1;
                else
                    step_check = 0;
                end
            end
            fprintf('\n\nPlotting the desired trajectory and sending to the PIC32 ...');

            fprintf(mySerial,'%d\n',length(ref_step));

            for step_index = 1:length(ref_step)
                fprintf(mySerial, '%d\n',ref_step(step_index));
            end
            fprintf(' completed.\n\n');

        case 'n'
            sample_cubic = [0,0;1,180;2,-180;3,0;4,360;5,300;8,-720];
            assignin('base','sample_cubic',sample_cubic);
            cubic_check = 1;
            while (cubic_check)
                ref_cubic = input('Enter cubic trajectory, in sec and degrees [time1, ang1; time2, ang2; ...]:\n\n');
                ref_cubic = genRef(ref_cubic, 'cubic');
                if (length(ref_cubic)>2001)
                    fprintf('Error: Maximum trajectory time is 10 seconds.\n\n')
                    close
                    cubic_check = 1;
                else
                    cubic_check = 0;
                end
            end
            fprintf('\n\nPlotting the desired trajectory and sending to the PIC32 ...');
            fprintf(mySerial, '%d\n',length(ref_cubic));

            for cubic_index = 1:length(ref_cubic)
                fprintf(mySerial, '%f\n',ref_cubic(cubic_index));
            end
            fprintf(' completed.\n\n');

        case 'o'

            nsamples = fscanf(mySerial,'%d');       % first get the number of samples being sent
            data = zeros(nsamples,2);               % two values per sample:  ref and actual
            for i=1:nsamples
                data(i,:) = fscanf(mySerial,'%f %f'); % read in data from PIC32; assume ints, in mA
                times(i) = (i-1);                 % 0.2 ms between samples
            end
            if nsamples > 1
                stairs(times/2,data(:,1:2));            % plot the reference and actual
            else
                fprintf('Only 1 sample received\n')
                disp(data);
            end
            % compute the average error
            score = mean(abs(data(:,1)-data(:,2)));
            fprintf('\nAverage error: %5.1f degrees\n\n\n',score);
            title(sprintf('Average error: %5.1f degrees',score));
            ylabel('Angle (degrees)');
            xlabel('Time (ms)');




        case 'p'


        case 'q'
            has_quit = true;             % exit client


        case 'r'
            r = fscanf(mySerial,'%d');
            switch r
                case 0
                    fprintf('The PIC32 controller mode is currently IDLE.\n\n\n');
                case 1
                    fprintf('The PIC32 controller mode is currently PWM.\n\n\n');
                case 2
                    fprintf('The PIC32 controller mode is currently ITEST.\n\n\n');
                case 3
                    fprintf('The PIC32 controller mode is currently HOLD.\n\n\n');
                case 4
                    fprintf('The PIC32 controller mode is currently TRACK.\n\n\n');
            end


        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end



























