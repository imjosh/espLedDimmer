$(document).ready(function(){
    $('div.slider').slider({
        orientation: 'vertical',
        value: [$("#value1").val()],
        min: 0,
        max: 255,
        range: 'min',
        animate: true,
        step: 1,
        slide: function (event, ui) {
            $("#value1").val(ui.value);
        },
        stop: function (event, ui) {
            $(this).closest("form").submit();
        }
    });

    $("#sub, #value1").hide();
});