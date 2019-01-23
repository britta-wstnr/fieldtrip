% FT_POSTAMBLE_SAVEVAR is a helper script that optionally saves the output
% FieldTrip data structures to a *.mat file on disk. This is useful for
% batching and for distributed processing. This makes use of the
% cfg.outputfile variable.
%
% Use as
%   ft_postamble savevar data
%   ft_postamble savevar source mri
%
% See also FT_PREAMBLE, FT_POSTAMBLE, FT_PREAMBLE_LOADVAR

% Copyright (C) 2011-2019, Robert Oostenveld, DCCN
%
% This file is part of FieldTrip, see http://www.fieldtriptoolbox.org
% for the documentation and details.
%
%    FieldTrip is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    FieldTrip is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with FieldTrip. If not, see <http://www.gnu.org/licenses/>.
%
% $Id$

if exist('Fief7bee_reproducescript', 'var')
  cfg.reproducescript = Fief7bee_reproducescript;
end

% the output data should be saved to a MATLAB file
if (isfield(cfg, 'outputfile') && ~isempty(cfg.outputfile)) || (isfield(cfg, 'reproducescript') && ~isempty(cfg.reproducescript))
  
  if isfield(cfg, 'outputlock') && ~isempty(cfg.outputlock)
    mutexlock(cfg.outputlock);
  end
  
  % WARNING: the following code is shared between ft_preamble_savefig and ft_preamble_savevar
  if isfield(cfg, 'reproducescript') && ~isempty(cfg.reproducescript)
    iW1aenge_now = datestr(now, 30);
    cfg.outputfile = {};
    for i=1:(ft_nargout)
      cfg.outputfile{i} = sprintf('%s_output_%s', iW1aenge_now, iW1aenge_postamble{i});
    end
    
    % write the script that reproduces the analysis
    iW1aenge_cfg = removefields(cfg.callinfo.usercfg, ignorefields('reproducescript'));
    iW1aenge_cfg = copyfields(cfg, iW1aenge_cfg, {'outputfile'});
    iW1aenge_cfg = printstruct('cfg', iW1aenge_cfg);
    iW1aenge_fid = fopen(cfg.reproducescript, 'a+');
    fprintf(iW1aenge_fid, "%%%%\n\n");
    fprintf(iW1aenge_fid, "cfg = [];\n");
    fprintf(iW1aenge_fid, "%s\n", iW1aenge_cfg);
    iW1aenge_st = dbstack(2);
    fprintf(iW1aenge_fid, '%s(cfg);\n\n', iW1aenge_st(1).name);
    fclose(iW1aenge_fid);
  end
  
  if isequal(iW1aenge_postamble, {'varargout'}) && ~iscell(cfg.outputfile)
    % this should be a cell-array, oterwise it cannot be matched with varargout
    cfg.outputfile = {cfg.outputfile};
  end
  
  if iscell(cfg.outputfile)
    % iW1aenge_postamble is a cell-array containing the variable names
    if isequal(iW1aenge_postamble, {'varargout'})
      % the output is in varargout
      for tmpindx=1:length(cfg.outputfile)
        savevar(cfg.outputfile{tmpindx}, 'data', varargout{tmpindx});
      end % for
      clear tmpindx
    else
      % the output is in explicitly named variables
      for tmpindx=1:length(cfg.outputfile)
        savevar(cfg.outputfile{tmpindx}, iW1aenge_postamble{tmpindx}, eval(iW1aenge_postamble{tmpindx}));
      end % for
      clear tmpindx
    end
  else
    % iW1aenge_postamble{1} contains the name of the only variable
    savevar(cfg.outputfile, iW1aenge_postamble{1}, eval(iW1aenge_postamble{1}));
  end
  
  if isfield(cfg, 'outputlock') && ~isempty(cfg.outputlock)
    mutexunlock(cfg.outputlock);
  end
  
  if ~ft_nargout
    % do not return the output variable "ans"
    clear(iW1aenge_postamble{1});
  end
end

