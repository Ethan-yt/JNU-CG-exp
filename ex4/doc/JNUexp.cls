\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{JNUexp}[08/11/2017, v1.0]
\DeclareOption*{\PassOptionsToClass{\CurrentOption,11pt,a4paper}{article}}
\ProcessOptions*
\LoadClass{article}

%-------------------------------------------------------------------------------
%   FONT
%-------------------------------------------------------------------------------
\RequirePackage{fontspec}
\setmainfont{Times New Roman}
\RequirePackage{ctex}

\setlength\parindent{0pt} % Removes all indentation from paragraphs
\renewcommand{\baselinestretch}{1.0} % 行距

%-------------------------------------------------------------------------------
%   MARGIN
%-------------------------------------------------------------------------------

\RequirePackage[a4paper,%
left=2.5cm,%
right=2.5cm,%
top=2.25cm,%
bottom=2.25cm%
]{geometry}%

%-------------------------------------------------------------------------------
%   HEADER and FOOTER
%-------------------------------------------------------------------------------
% Set up the header and footer
\RequirePackage{fancyhdr}  			% use fancyhdr package to get 2-line header
\pagestyle{fancy}\fancyhf{}
\lhead{} % Top left header
\chead{\@border\@courseName: \@expName} % Top center head
\rhead{} % Top right header
\lfoot{} % Bottom left footer
\cfoot{} % Bottom center footer
\rfoot{Page\ \thepage\ of\ \protect\pageref{LastPage}} % Bottom right footer
\renewcommand\headrulewidth{0.4pt} % Size of the header rule
\renewcommand\footrulewidth{0.4pt} % Size of the footer rule
\fancyheadoffset{0.3 cm} 
\fancyfootoffset{0.3 cm} 
%-------------------------------------------------------------------------------
%   BORDER
%-------------------------------------------------------------------------------

\RequirePackage{tikz}
\usetikzlibrary{calc}

\newcommand{\@border}{
\begin{tikzpicture}[remember picture,overlay] 
\draw [line width=0.4pt]
($ (current page.south west) + (2.2cm,2cm) $)
rectangle
($ (current page.north east) + (-2.2cm,-2cm) $);
\end{tikzpicture}
}

\newcommand{\@beginLine}{
\begin{tikzpicture}[remember picture,overlay] 
\draw [line width=0.4pt]
($ (current page.north west) + (2.2cm,-5.2cm) $)
--
($ (current page.north east) + (-2.2cm,-5.2cm) $);
\end{tikzpicture}
}
\newcommand{\@endLine}{
\begin{tikzpicture}[remember picture,overlay] 
\draw [line width=0.4pt]
($ (current page.south west) + (2.2cm,4cm) $)
--
($ (current page.south east) + (-2.2cm,4cm) $);
\end{tikzpicture}
}

%-------------------------------------------------------------------------------
%	BEGIN of DOCUMENT
%-------------------------------------------------------------------------------

\newcommand{\courseName}[1]{\def\@courseName{#1}}
\newcommand{\expName}[1]{\def\@expName{#1}}
\newcommand{\expDate}[1]{\def\@expDate{#1}}
\newcommand{\className}[1]{\def\@className{#1}}
\newcommand{\studentName}[1]{\def\@studentName{#1}}
\newcommand{\studentId}[1]{\def\@studentId{#1}}


\RequirePackage{ulem}
\newcommand{\@titleItem}[2]{#1 \uline{\hspace{\stretch{1}}#2\hspace{\stretch{1}}}}

\RequirePackage{tabularx}

\AtBeginDocument{
\@beginLine
{
\centering
{\fontsize{26pt}{26pt}\textbf{江南大学物联网工程学院实验报告}\\[5pt]}
\renewcommand{\arraystretch}{1.2} % 行间距
\renewcommand{\tabcolsep}{0.1cm} % 列间距
\begin{tabularx}{0.97\textwidth}{cXc}
\@titleItem{课程名称}{《\@courseName》} &
\@titleItem{实验名称}{\@expName} &
\@titleItem{实验日期}{\@expDate} \\
\@titleItem{班级}{\@className} &
\@titleItem{姓名}{\@studentName} &
\@titleItem{学号}{\@studentId} \\
\multicolumn{3}{l}{实验报告要求   1．实验目的  2．实验内容  3．实验步骤   4．运行情况  5．实验体会 }
\end{tabularx}
}
}

%-------------------------------------------------------------------------------
%	END
%-------------------------------------------------------------------------------


\AtEndDocument{


\@endLine
\begin{figure}[b!]
    \renewcommand\arraystretch{1.5} % 行间距
    \renewcommand{\tabcolsep}{0.2cm} % 列间距
    \newcolumntype{C}{ >{\centering\arraybackslash} m{1.5cm} }
    \newcolumntype{D}{ >{\centering\arraybackslash} m{0.3cm} }
    \newcolumntype{E}{ >{\centering\arraybackslash} m{0.6cm} }
    \begin{tabularx}{\textwidth}{C|D|X|D|X|D|X|D|X|D|X|E|p{2cm}|D|p{2cm}}
    教师评价 & 优 & & 良 & & 中 & & 及 格 & & 不 及 格& & 教师 签名 & & 日 期 & \\
    \end{tabularx}
    \vspace*{-7.5pt}   
\end{figure}



}
% Put this command here to avoid ending in an extra page
\RequirePackage{lastpage} % Required to determine the last page for the footer

%-------------------------------------------------------------------------------
%	CODE INCLUSION CONFIGURATION
%-------------------------------------------------------------------------------


% https://en.wikibooks.org/wiki/LaTeX/Source_Code_Listings#Supported_languages
% http://texdoc.net/texmf-dist/doc/latex/listings/listings.pdf

\RequirePackage{listings}

\newfontfamily\menlo{Menlo}

\lstset{
    columns=fixed,    
    breaklines=true,   
    numbers=left,                                        % 在左侧显示行号
    frame=single,                                        % 显示背景边框
    backgroundcolor=\color[RGB]{245,245,244},            % 设定背景颜色
    % keywordstyle=\color[RGB]{40,40,255},                 % 设定关键字颜色
    numberstyle=\footnotesize,           % 设定行号格式
    % commentstyle=\color[RGB]{0,96,96},                   % 设置代码注释的格式
    % stringstyle=\color[RGB]{128,0,0},                    % 设置字符串格式
    showstringspaces=false,                              % 不显示字符串中的空格
    basicstyle=\small\menlo,
    numberstyle=\scriptsize\menlo,
    xleftmargin=1cm,
    xrightmargin=1cm
}

%-------------------------------------------------------------------------------
%	DOCUMENT STRUCTURE COMMANDS
%-------------------------------------------------------------------------------
\newcounter{homeworkProblemCounter} % Creates a counter to keep track of the number of problems

\newenvironment{problem}{
\stepcounter{homeworkProblemCounter} % Increase counter for number of problems
\paragraph{Problem \arabic{homeworkProblemCounter}}
}{\par}

\newenvironment{answer}{
    % \paragraph{Answer}\mbox{}\\
    \bigskip
}{}

\RequirePackage{graphicx}

\newenvironment{image}{
    \subparagraph{运行截图} \mbox{}\\
    \begin{figure}[h]
    \centering
}{\end{figure}}
