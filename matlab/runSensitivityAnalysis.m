close all;
clear;
clc;

MINX=-3.0;
MAXX=1.0;

MINY=-6.0;
MAXY=6.0;

%at this height, the realsense enters the icub camera FOV when eyes' tilt go beyond 2.0 degrees
MINZ=-10.0;
MAXZ=0.0;

MINTHETA=0.0;
MAXTHETA=25.0; %at 30, the realsense sees part of the icub head
THETASTEP=5.0;
NTHETA=(MAXTHETA-MINTHETA)/THETASTEP;

THETA_0=-0.5249;

CURR_HOLDER_POS=[-0.084112 0.0 0.4625];

% we save result here
root=getenv('ROBOT_CODE')+"/design-holder-realsense";
if ~exist(root,'dir')
   warning('Could not find %s',root);
   return;
end
filename=root+"/result-test-new-2.txt";

if (isfile(filename))
    result=importdata(filename);
else
    warning('Could not open %s',filename);
    return;
end

%convert to cm
result(:,1:3)=result(:,1:3).*100;

%switch x with y
tmp=result(:,1);
result(:,1)=result(:,2);
result(:,2)=tmp;

poses=result(:,1:4); %+startin_pos;
x=poses(:,1);
y=poses(:,2);
z=poses(:,3);
theta=poses(:,4);
fit_score=result(:,5);

subpos=zeros(size(poses,1),5);
fin_poses=[];
j=1;
while (j < size(poses,1))
    subpos=result(j:j+NTHETA,:);
    [maxv,k] = min(fit_score(j:j+NTHETA));
    fin_poses = [subpos(k,:); fin_poses];
    j = j+NTHETA+1;
end

%% visualization
% icub_head_stl=root+"/assets/head.stl";
% model = createpde('structural','static-solid');
% importGeometry(model,icub_head_stl);
% pdegplot(model);

icub_head=root+"/assets/icub_head.png";

figure(1); 
img_size = 20;
y_shift = -0.4;
z_shift = -2;
x_shift = 0.0;
[img, map, alphachannel] = imread(icub_head);
img = flip(img,1);
alphachannel = flip(alphachannel,1);

quiver3(0.0,0.0,0.0,0.0,0.0,0.0,'Color','white'); hold on;
set(gca, 'xdir', 'reverse');
img = flip(img,2);
alphachannel = flip(alphachannel,2);
surface([-img_size+y_shift img_size+y_shift; -img_size+y_shift img_size+y_shift], [x_shift x_shift; x_shift x_shift], ...
    [-img_size+z_shift -img_size+z_shift; img_size+z_shift img_size+z_shift], ...
    'FaceColor', 'texturemap', 'CData', img, ... 
    'FaceAlpha', 'texturemap', 'AlphaData', alphachannel, ...
    'EdgeColor', 'none');
R=axang2rotm([1 0 0 1.5*pi+THETA_0]);
trans_0=[0.0 -1.5 11.584];
cam=rigid3d(R,trans_0);
plotCamera('AbsolutePose',cam,'Size',0.5,'Opacity',0.1,'Color','red'); 

levels=10;
cmap=jet(levels);
scoremap=zeros(size(fin_poses,1),4);
for i = 1 : size(fin_poses,1)
    score=fin_poses(i,5);
    scoremap(i,1)=score;
    if (score<=0.003)
        scoremap(i,2:end)=cmap(1,:);
    else
        if (score>0.003 && score<=0.02)
            scoremap(i,2:end)=cmap(2,:);
        else            
            if (score>0.02 && score<=0.2)
                scoremap(i,2:end)=cmap(3,:);
            else
                if (score>0.2 && score<=30.0)
                    scoremap(i,2:end)=cmap(4,:);
                else
%                 if (score>30.0 && score<=40.0)
%                     scoremap(i,2:end)=cmap(4,:);
%                 else
%                     if (score>40.0 && score<=50.0)
%                         scoremap(i,2:end)=cmap(5,:);
%                     else
%                         if (score>50.0 && score<=60.0)
%                             scoremap(i,2:end)=cmap(6,:);
%                         else
%                             if (score>60.0 && score<=70.0)
%                                 scoremap(i,2:end)=cmap(7,:);
%                             else
%                                 if (score>70.0 && score<=80.0)
%                                     scoremap(i,2:end)=cmap(8,:);
%                                 else
%                                     if (score>80.0 && score<=90.0)
%                                         scoremap(i,2:end)=cmap(9,:);
%                                     else
%                                         if (score>90.0 && score<=100.0)
%                                             scoremap(i,2:end)=cmap(10,:);
%                                         end
%                                     end
%                                 end
%                             end
%                         end
%                     end
                 end
            end
        end
    end            
end


for i = 1 : size(fin_poses,1)
    score=fin_poses(i,5);    
    color=scoremap(scoremap(:,1)==score,2:end);
    if(size(color,1) > 1)
        color = color(1, :);
    end
    R1=axang2rotm([1 0 0 1.5*pi+THETA_0]);
    R2=axang2rotm([1 0 0 -(pi/180)*theta(i)]);
    R=R2*R1;
    trans=fin_poses(i,1:3)+trans_0;
    cam=rigid3d(R,trans);
    plotCamera('AbsolutePose',cam,'Size',0.5,'Opacity',0.1,'Color',color);
     text(trans(1),trans(2),trans(3)+0.5,num2str(score),'HorizontalAlignment','left', ...
         'VerticalAlignment', 'bottom','FontSize', 12, 'Color', color);
end

view(-47.3,21.8);
hold off;
axis equal;
xlabel('Y range [cm]');
ylabel('X range [cm]');
zlabel('Z range [cm]');
%xlim([-12 10]);
xlim([-15 10]); 
ylim([-10 1.0]);
%zlim([-18 20]);
zlim([-8 20]); 

% scatter3(fin_poses(:,2), fin_poses(:,1), fin_poses(:,3), 20, fin_poses(:,5),'filled');
% set(gca,'xdir','reverse');

%38: -0.03 -0.04 -0.10 0.0 6.09999999999999964473
%39: -0.03 -0.04 -0.10 5.0 12.6999999999999992895
