function listdirs () {

    var url = new URL(location.toString());
    var checked = url.searchParams.getAll('dirs[]');

    $.ajax({
        url: 'ls.php',
        success: function (data) {
            var first = document.getElementById("firstdirs");
            first.innerHTML = '';
            var all = document.getElementById("alldirs");
            all.innerHTML = '';
            var show = 20;
            data.forEach(function (dir) {
                var input = document.createElement('input');
                input.type = 'checkbox';
                input.name = 'dirs[]';
                input.value = dir;
                if (checked.indexOf(dir) >= 0)
                    input.checked = true;
                var div = ((--show >= 0) ? first : all);

                var link = document.createElement('a');
                link.href = '../plots/' + dir + '/models.cnf';
                link.innerHTML = 'models';

                div.appendChild(link);
                div.appendChild(input);
                div.appendChild(document.createTextNode(dir));
                div.appendChild(document.createElement('br'));
            });
        },
    });

};


$(document).ready(function(){
    $("#showalldirs").click(function() {

        if ($("#showalldirs").html() == "Show All Directories") {
            $("#showalldirs").html("Hide All Directories")
        } else {
            $("#showalldirs").html("Show All Directories")
        }

        $("#alldirs").toggle();
        $("#allmessage").toggle();
    });

    var dirs = $("#dirs");

    $("#showdirs").click(function() {
        if (dirs.get(0).style.display == 'none')
            listdirs();

        dirs.toggle();
    });

    if (dirs.get(0).style.display != 'none')
        listdirs();

});

