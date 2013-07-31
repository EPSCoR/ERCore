(function($, window, document, undefined){
    var fullMonthNames = [ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" ];
    var monthNames = [ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" ];
    Math.randSeed = function(n){
        return Math.abs(Number(new Date(n%9999, n%12, n%30, n%24, n%60, n%60, n%1000)));
    };
    function longDateFormat(date){
        return fullMonthNames[date.getMonth()] + " " + date.getDate() + ", " + date.getFullYear();
    }
    function shortDateFormat(date){
        return monthNames[date.getMonth()] + " " + date.getDate() + ", " + date.getFullYear();
    }
    window.log = function (){
        if (console && console.log)
            console.log.apply(console, arguments);
    }
    function decomposeDate(date){
        var m = date.getMonth(), y = date.getFullYear();
        return {
            'd': date.getDate(),
            'm': m,
            'yy': y.toString().substring(2),
            'yyyy': y,
            'mon': monthNames[m],
            'month': fullMonthNames[m]
        };
    }
    //compiles simple templates: written by andrew wessels :)
    function compileTpl(str){
        str = str.replace(/'/, "\\'");
        var m = str.match(/{{[^{}]+(\.[^{}]+)?}}/g);
        for (var i in m) str = str.replace(m[i], m[i].replace(/\./, "']['").replace(/{{/, "'+d['").replace(/}}/, "']+'"));
        return new Function("d", "return '"+str+ "';");
        //return new Function("d", "return '"+str.replace(/{{/g, "'+d['").replace(/}}/g, "']+'")+ "';");
    }
    //test:
    //var test = "{ok}{{test.blah}} {{foo}}, {{bar}}{ok\'s}";
    //log(test, "=>", compileTpl(test));
        
        
    //var blocks = <?=$series_json?>,
    //    settings = <?=$info_json?>,
    //    $stage = $("#chart-area-<?=$chartID?>");
    //new timeline($stage, blocks, settings);
    var today = new Date(),
        startOfEpscor = Drupal.settings.ER_START_DATE?Drupal.settings.ER_START_DATE:new Date('9/15/2009'),
        minRows = 3,
        barHeight = 14,
        barPadding = 3,
        textPadding = barHeight/2,
        pagePadding = 15,
        spacer = barHeight+barPadding,
        W = 780;
    
    window.Timeline = function ($stage, blocks, settings) {
      //DEBUG
      log('blocks', blocks);
      log('settings', settings);

      var minDate = settings['minDate'] ? new Date(settings['minDate'] * 1000) : startOfEpscor,
        maxDate = settings['maxDate'] ? new Date(settings['maxDate'] * 1000) : today,
        displayTodayLine = (maxDate - today) > 3600000; //more than a hour in the future
      //displayAlpha = true;
      //maxDate = useToday?today:new Date('9/15/2012');//new Date(<?=$series['max']?>*1000);

      var count = 0;
      for (var i in blocks) {
        count += Math.max(blocks[i].data.length, minRows);
      }

      var H = count * spacer + 100,
        paper = Raphael($stage.find('.timeline')[0], W, H),
        scale = (W - pagePadding * 2) / (maxDate - minDate);

      function getX(date) {
        return pagePadding + (date - minDate) * scale;
      }

      //tooltip code
      var $tooltip = $stage.find('div#tooltip'),
        tooltipDelay = 250,
        tooltipHider,
        hideTooltip = function () {
          $tooltip.hide();
        };
      $tooltip.hover(function () {
        clearInterval(tooltipHider);
      }, function () {
        tooltipHider = setTimeout(hideTooltip, tooltipDelay);
      });

      //DRAW DATES AND YEAR RULERS
      var date = new Date(minDate.getFullYear(), minDate.getMonth() + 1, 1, 0, 0, 0, 0),
        endDate = maxDate, //new Date(maxDate.getFullYear(), maxDate.getMonth()+1, 1, 0, 0, 0, 0),
        labelY = H - 90,
        first = true,
        years = maxDate.getFullYear() - minDate.getFullYear(),
        showMonths = years < 6,
        skipYears = years ? Math.ceil(years / 10) : 1;

      //paper.path("M"+15+" 0L"+15+" "+4095).attr({'stroke-width':1, 'stroke-dasharray': '. ', 'stroke':'#000'});

      while (date <= endDate) {
        //log(monthNames[date.getMonth()], date.getFullYear());
        //log(date.getTime()/1000, min, (date.getTime()/1000)-min, ((date.getTime()/1000)-min)*scale);
        var labelX = parseInt(getX(date)),
          curMonth = date.getMonth();
        if (showMonths) {
          paper.path("M" + labelX + " " + labelY + "L" + labelX + " " + (labelY - 5)).attr('stroke-width', 1);
          paper.text(labelX - 10, labelY + 10, monthNames[curMonth]).attr({'text-anchor': 'start'}).transform("r45");
        }
        if (curMonth == 0) {//january
          paper.path("M" + labelX + " 0L" + labelX + " " + (labelY - 5)).attr({'stroke-width': 2, 'stroke-dasharray': '. ', 'stroke': '#ddd'});
        }
        date.setMonth(curMonth + 1);
        var year = date.getFullYear(),
          year_y = labelY + (showMonths ? 30 : 10);
        if (!(year % skipYears)) {
          if (curMonth < 7 && date > endDate) {
            paper.text(labelX, year_y, year).attr({'font-size': 16, 'text-anchor': 'end'});
          } else if (first && curMonth > 7) {
            paper.text(labelX, year_y, year).attr({'font-size': 16, 'text-anchor': 'start'});
          } else if (curMonth == 7) {
            paper.text(labelX, year_y, year).attr({'font-size': 16});
          }
        }
        first = false;
      }

      //DRAW REPORTING YEAR RULERS
      var date = new Date(startOfEpscor);//new Date(minDate.getFullYear(), minDate.getMonth()+1, 1, 0, 0, 0, 0);
      var first = true;
      while (date <= maxDate) {
        var curYear = date.getFullYear();
        if (date > minDate) {
          var labelX = getX(date);
          var color = first ? 'red' : 'teal';
          paper.path("M" + labelX + " 0L" + labelX + " " + (labelY - 5)).attr({'stroke-width': 1, 'stroke-dasharray': '- ', 'stroke': color});
        }
        if (first) {
          date.setDate(1); //reporting periods start at the first of the month
          first = false;
        }
        date.setFullYear(curYear + 1);
      }

      if (displayTodayLine) {//only displays the today line if the maxDate is more than an hour in the future.
        var labelX = getX(today);
        paper.path("M" + labelX + " 0L" + labelX + " " + (labelY - 5)).attr({'stroke-width': 1, 'stroke': '#11cc11'});
      }


      var row = 0,
        multiBlocks = blocks.length > 1;
      for (var bid in blocks) {
        if (multiBlocks) {
          var y = row * spacer,
            numRows = Math.max(minRows, blocks[bid].data.length);
          paper.text(10, y + 15, blocks[bid].info['name']).attr({'font-size': 24, 'font-weight': 'bold', 'text-anchor': 'start', 'fill': 'silver'}).toBack();
          //This inserts a blue background color on the odd blocks
          if (bid & 1) {
            var x = -2,//shift it off the screen a bit to hide the left and right borders
              height = numRows * spacer;
            width = W + 4;
            paper.rect(x, y, width, height, 0).attr({'fill': '#EDF5FA', 'stroke': '#D3E7F4'}).toBack();
          }
        }
        (function (info, data) {
          //this generates a template function which fills the title of the tooltip popup
          var titleTpl = compileTpl(info['tooltip-title']),
            bodyTpl = compileTpl(info['tooltip-body']),
            pillTpl = compileTpl(info['pill-label']);
          //log(titleTpl);

          for (var i in data) {
            //log(data[i]);
            //log('x:', x, 'y:', y, 'width:', barWidth);
            (function (id, data) {
              var rnd = Math.randSeed(data[info['pill-color']]).toString(), //hash used to create colors
                r = Number(rnd.substring(0, 3)) % 255,//red
                g = Number(rnd.substring(3, 6)) % 255,//green
                b = Number(rnd.substring(6, 9)) % 255,//blue
                date = data['date'] ? new Date(data['date'] * 1000) : today,//new Date(data['date']*1000),
                date2 = data['date2'] ? new Date(data['date2'] * 1000) : today,
                dateString = shortDateFormat(date) + " - " + shortDateFormat(date2) + (data['date2'] ? '' : '+'),
                barWidth = Math.max((date2 - date) * scale, 15), //width of pill
                x = getX(date), //x-pos of pill
                y = 1 + row * spacer; //y-pos of pill
              row++;
              //log(data);
              //add in some date information for use in templates.
              //data["date-format"] = [];
              //var m = date.getMonth();
              if (data['date'] == null && data['date2'] == null) {
                dateString = 'Invalid date! No start date.';
              }
              //data["date"] = $.extend({'raw':data['date']}, decomposeDate(date));
              //data["date2"] = $.extend({'raw':data['date2']}, decomposeDate(date2), {'+':data['date2']?'':'+'});//decomposeDate(date2);
              //data["date2-format"]['+'] = data['date2']?'':'+';

              //draw the elements
              var rect = paper.rect(x, y, barWidth, barHeight, 10).attr('fill', "rgb(" + r + "," + g + "," + b + ")").attr('href', data['nlink']),
                label = paper.text(x + barWidth / 2, y + textPadding, pillTpl(data)).attr('href', data['nlink']),
                bar = paper.set().push(rect, label).attr('class', 'pill');
              $(label[0]).children('tspan').attr('dy', 4); //for some reason the text isn't being positioned properly...?

              //can use this to remove the pills later:
              //rect.node.setAttribute("class", "pill");
              //label.node.setAttribute("class", "pill");

              //positioning for tooltip
              var bbb = rect.getBBox(),
              //ofs = $stage.offset(),
                tooltipX = bbb.x + bbb.width / 2 - $tooltip.width() / 2,
                tooltipY = bbb.y - bbb.height - 2;
              bar.hover(function () {
                clearInterval(tooltipHider);
                //log(data);
                $tooltip.find('.title').html(titleTpl(data));//.html('<a href="'+data['ulink']+'">'+data['lname']+", "+data['fname']+'</a>');
                $tooltip.find('.dates').html(dateString);
                $tooltip.find('.body').html(bodyTpl(data));
                $tooltip.show().css({
                  left: tooltipX,
                  top: tooltipY - $tooltip.height()
                });
              }, function () {
                tooltipHider = setTimeout(hideTooltip, tooltipDelay);
              });

              //label.mouseover(function(e){
              //	log('over', e);
              //}).mouseout(function(e){
              //	log('out', e);
              //});
            })(i, data[i]);
            //bar.on = true;
            //log(bar, bar.on);
          }
        })(blocks[bid].info, blocks[bid].data);
        if (multiBlocks)
          row += Math.max(minRows - blocks[bid].data.length, 0);//ensure that one section takes up at least 3 rows.
      }
    };
        //var maxlen = <?=isset($info['maxlen'])?$info['maxlen']:23?>; //chops off the tooltip text if it's too damn long.
})(jQueryNew, this, document);